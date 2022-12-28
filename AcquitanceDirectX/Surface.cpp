#define FULL_WINTARD
#include "Surface.h"
#include <algorithm>
#include <filesystem>
#include "Window.hpp"


#include <sstream>



Surface::Surface(unsigned int width, unsigned int height, unsigned int pitch) noexcept
{
	HRESULT hr = scratch.Initialize2D(format, width, height, 1u, 1u);
	if (FAILED(hr))
	{
		throw Surface::Exception(__LINE__, __FILE__, "Failed to initialize ScratchImage",  hr);
	}
}



Surface::Surface(unsigned int width, unsigned int height) noexcept
	:
	Surface(width, height, width)
{}

Surface::Surface(DirectX::ScratchImage scratch_in) noexcept
	:
	scratch(std::move(scratch_in))
{}



void Surface::Clear(Color fillValue) noexcept
{
	int width = GetWidth();
	int height = GetHeight();
	auto& ImageData = *scratch.GetImage(0,0,0);
	for (size_t y = 0; y < height; y++)
	{
		auto rowBegin = reinterpret_cast<Color*>(ImageData.pixels + ImageData.rowPitch * y);
		std::fill(rowBegin, rowBegin + width, fillValue);
	}
	
}

void Surface::PutPixel(unsigned int x, unsigned int y, Color c) noxnd
{
	int width = GetWidth();
	int height = GetHeight();
	assert(x >= 0);
	assert(y >= 0);
	assert(x < width);
	assert(y < height);
	auto& ImageData = *scratch.GetImage(0, 0, 0);
	reinterpret_cast<Color*>(&ImageData.pixels[ImageData.rowPitch*y])[x] = c;
	
}

Surface::Color Surface::GetPixel(unsigned int x, unsigned int y) const noxnd
{
	int width = GetWidth();
	int height = GetHeight();
	assert(x >= 0);
	assert(y >= 0);
	assert(x < width);
	assert(y < height);
	auto& ImageData = *scratch.GetImage(0, 0, 0);
	return reinterpret_cast<Color*>(&ImageData.pixels[ImageData.rowPitch * y])[x];
}

unsigned int Surface::GetWidth() const noexcept
{
	return scratch.GetMetadata().width;
}

unsigned int Surface::GetHeight() const noexcept
{
	return scratch.GetMetadata().height;
}

Surface::Color* Surface::GetBufferPtr() noexcept
{
	return reinterpret_cast<Color*>(scratch.GetPixels());
}

const Surface::Color* Surface::GetBufferPtr() const noexcept
{
	return const_cast<Surface*>(this)->GetBufferPtr();
}

const Surface::Color* Surface::GetBufferPtrConst() const noexcept
{
	return const_cast<Surface*>(this)->GetBufferPtr();
}

Surface Surface::FromFile(const std::string& name)
{
	wchar_t wideName[512];
	mbstowcs_s(nullptr, wideName, name.c_str(), _TRUNCATE);

	auto scratch = DirectX::ScratchImage{};
	HRESULT hr = DirectX::LoadFromWICFile(wideName, DirectX::WIC_FLAGS_NONE, nullptr, scratch);

	if (FAILED(hr))
	{
		throw Surface::Exception(__LINE__, __FILE__, "Failed to create ScratchImage", hr);
	}

	if (scratch.GetImage(0, 0, 0)->format != format)
	{
		auto converted = DirectX::ScratchImage{};
		HRESULT hr = DirectX::Convert(*scratch.GetImage(0,0,0), format, DirectX::TEX_FILTER_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, converted);
		if (FAILED(hr))
		{
			throw Surface::Exception(__LINE__, __FILE__, "Failed to convert the format", hr);

		}
		return Surface(std::move(converted));
	}
	return Surface(std::move(scratch));

	
}

void Surface::Save(const std::string& filename) const
{
	const auto GetCodecID = [](const std::string& filename) {
		const std::filesystem::path path = filename;
		const auto ext = path.extension().string();
		if (ext == ".png")
		{
			return DirectX::WIC_CODEC_PNG;
		}
		else if (ext == ".jpg")
		{
			return DirectX::WIC_CODEC_JPEG;
		}
		else if (ext == ".bmp")
		{
			return DirectX::WIC_CODEC_BMP;
		}
		throw Exception(__LINE__, __FILE__, filename, "Image format not supported");
	};

	wchar_t wideName[512];
	mbstowcs_s(nullptr, wideName, filename.c_str(), _TRUNCATE);

	HRESULT hr = DirectX::SaveToWICFile(
		*scratch.GetImage(0, 0, 0),
		DirectX::WIC_FLAGS_NONE,
		GetWICCodec(GetCodecID(filename)),
		wideName
	);
	if (FAILED(hr))
	{
		throw Surface::Exception(__LINE__, __FILE__, filename, "Failed to save image", hr);
	}
}




// surface exception stuff
Surface::Exception::Exception(int line, const char* file, std::string note, std::string filename, std::optional<HRESULT> hr) noexcept
	:
	ChiliException(line, file)

{
	using namespace std::string_literals;
	note = "[FileName]"s + filename + " "s + "[Note]"s + note;
	if (hr)
	{
		note = "[HRESULT ERROR STRING]"s + Window::Exception::TranslateErrorCode(*hr) + note;
	}
}
Surface::Exception::Exception(int line, const char* file, std::string note, std::optional<HRESULT> hr) noexcept
	:
	ChiliException(line, file)

{
	using namespace std::string_literals;
	note = "[FileName]"s + std::string(file) + " "s + "[Note]"s + note;
	if (hr)
	{
		note = "[HRESULT ERROR STRING]"s + Window::Exception::TranslateErrorCode(*hr) + note;
	}
}

const char* Surface::Exception::what() const noexcept
{
	std::ostringstream oss;
	oss << ChiliException::what() << std::endl
		<< "[Note] " << GetNote();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Surface::Exception::GetType() const noexcept
{
	return "Chili Graphics Exception";
}

const std::string& Surface::Exception::GetNote() const noexcept
{
	return note;
}

bool Surface::AlphaLoaded() const noexcept
{
	return !scratch.IsAlphaAllOpaque();
}