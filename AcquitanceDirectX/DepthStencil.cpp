#include "DepthStencil.h"
#include "Surface.h"

DXGI_FORMAT MapUsageTypeless(DepthStencil::Usage usage)
{
	switch (usage)
	{
	case DepthStencil::Usage::DepthStencilUsage:
		return DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS;
	case DepthStencil::Usage::ShadowDepthUsage:
		return DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS;
	}
	throw std::runtime_error{ "Base usage for Typeless format map in DepthStencil." };
}

DXGI_FORMAT MapUsageTyped(DepthStencil::Usage usage)
{
	switch (usage)
	{
	case DepthStencil::Usage::DepthStencilUsage:
		return DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
	case DepthStencil::Usage::ShadowDepthUsage:
		return DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
	}
	throw std::runtime_error{ "Base usage for Typed format map in DepthStencil." };
}

DXGI_FORMAT MapUsageColored(DepthStencil::Usage usage)
{
	switch (usage)
	{
	case DepthStencil::Usage::DepthStencilUsage:
		return DXGI_FORMAT::DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	case DepthStencil::Usage::ShadowDepthUsage:
		return DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
	}
	throw std::runtime_error{ "Base usage for Colored format map in DepthStencil." };
}

DepthStencil::DepthStencil(Graphics& gfx, UINT width, UINT height, Usage usage)
	:
	width{ static_cast<int>(width)},
	height{ static_cast<int>(height)}
{
    INFOMAN(gfx);

    // create depth stensil texture
    Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencil;
    D3D11_TEXTURE2D_DESC descDepth = {};
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1u;
    descDepth.ArraySize = 1u;
    descDepth.Format = MapUsageTypeless(usage);
    descDepth.SampleDesc.Count = 1u;
    descDepth.SampleDesc.Quality = 0u;// antialiacing
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(&descDepth, nullptr, &pDepthStencil));

	D3D11_DEPTH_STENCIL_VIEW_DESC descView = {};
	descView.Format = MapUsageTyped(usage);
	descView.Flags = 0;
	descView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descView.Texture2D.MipSlice = 0;

    GFX_THROW_INFO(GetDevice(gfx)->CreateDepthStencilView(pDepthStencil.Get(), &descView, &pDSView));
}

void DepthStencil::Bind(Graphics& gfx) noexcept 
{
    GetContext(gfx)->OMSetRenderTargets(0, nullptr, pDSView.Get());
}

void DepthStencil::Clear(Graphics& gfx) const noexcept 
{
    GetContext(gfx)->ClearDepthStencilView(pDSView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u);
}

Surface DepthStencil::ToSurface(Graphics& gfx, bool lineralze) const
{
	namespace wrl = Microsoft::WRL;

	// creating a temp texture compatible with the source, but with CPU read access
	wrl::ComPtr<ID3D11Resource> pResSource;
	pDSView->GetResource(&pResSource);
	wrl::ComPtr<ID3D11Texture2D> pTexSource;
	pResSource.As(&pTexSource);
	D3D11_TEXTURE2D_DESC textureDesc;
	pTexSource->GetDesc(&textureDesc);
	textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	textureDesc.Usage = D3D11_USAGE_STAGING;
	textureDesc.BindFlags = 0;
	wrl::ComPtr<ID3D11Texture2D> pTexTemp;
	GetDevice(gfx)->CreateTexture2D(
		&textureDesc, nullptr, &pTexTemp
	);

	// copy texture contents
	GetContext(gfx)->CopyResource(pTexTemp.Get(), pTexSource.Get());

	// create Surface and copy from temp texture to it
	const auto width = GetWidth();
	const auto height = GetHeight();
	Surface s{ width,height };
	D3D11_MAPPED_SUBRESOURCE msr = {};
	GetContext(gfx)->Map(pTexTemp.Get(), 0, D3D11_MAP::D3D11_MAP_READ, 0, &msr);
	auto pSrcBytes = static_cast<const char*>(msr.pData);
	for (unsigned int y = 0; y < height; y++)
	{
		struct Pixel
		{
			char indent[4];
		};
		auto pSrcRow = reinterpret_cast<const Pixel*>(pSrcBytes + msr.RowPitch * size_t(y));
		for (unsigned int x = 0; x < width; x++)
		{
			if (textureDesc.Format == DXGI_FORMAT_R24G8_TYPELESS)
			{
				const auto raw = 0xFFFFFF & *reinterpret_cast<const unsigned int*>(pSrcRow + x);
				unsigned char color = raw >> 16;
				if (lineralze)
				{
					const auto rawNormalized = (float)raw / (float)0xFFFFFF;
					const auto linearized = 0.01 / (1.01 - rawNormalized);
					color = linearized * 255;
				}
				s.PutPixel(x, y, { color, color, color });
			}
			else if (textureDesc.Format == DXGI_FORMAT_R32_FLOAT)
			{
				auto raw = *reinterpret_cast<const float*>(pSrcRow + x);
				unsigned char color = raw * 255;
				if (lineralze)
				{
					
					const auto linearized = 0.01 / (1.01 - raw);
					color = linearized * 255;
				}
				s.PutPixel(x, y, { color, color, color });
			}
			else
			{
				throw std::runtime_error{ "Bad format in Depth Stencil for conversion to Surface" };
			}
		}
	}
	GetContext(gfx)->Unmap(pTexTemp.Get(), 0);

	return s;
}

unsigned int DepthStencil::GetWidth() const
{
	return width;
}
unsigned int DepthStencil::GetHeight() const
{
	return height;
}