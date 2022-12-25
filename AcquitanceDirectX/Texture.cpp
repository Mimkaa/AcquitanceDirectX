#include "Texture.h"
#include "GraphicsThrowHeader.h"
#include "BindableCodex.h"
namespace Bind {
	Texture::Texture(Graphics& gfx, const std::string& path, unsigned int slot)
		:
		slot(slot),
		path(path)
	{
		INFOMAN(gfx);

		Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
		Surface surf = Surface::FromFile(path);
		
		hasAlphaSet = surf.AlphaLoaded();

		D3D11_TEXTURE2D_DESC tex_des = {};
		tex_des.Width = surf.GetWidth();
		tex_des.Height = surf.GetHeight();
		tex_des.MipLevels = 0u;
		tex_des.ArraySize = 1u;
		tex_des.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		tex_des.Usage = D3D11_USAGE_DEFAULT;
		tex_des.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		tex_des.CPUAccessFlags = 0u;
		tex_des.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
		tex_des.SampleDesc.Count = 1u;
		tex_des.SampleDesc.Quality = 0u;

		

		GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(&tex_des, nullptr, &pTexture));

		// write image data into top mip level
		GetContext(gfx)->UpdateSubresource(
			pTexture.Get(), 0u, nullptr, surf.GetBufferPtrConst(), surf.GetWidth() * sizeof(Surface::Color), 0u
		);


		D3D11_SHADER_RESOURCE_VIEW_DESC res_des = {};
		res_des.Format = tex_des.Format;
		res_des.Texture2D.MostDetailedMip = 0u;
		res_des.Texture2D.MipLevels = -1;
		res_des.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

		GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(pTexture.Get(), &res_des, &pTextureView));

		// generate the mip chain using the gpu rendering pipeline
		GetContext(gfx)->GenerateMips(pTextureView.Get());

	}

	bool Texture::HasAlpha() const noexcept
	{
		return hasAlphaSet;
	}

	void Texture::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->PSSetShaderResources(slot, 1u, pTextureView.GetAddressOf());
	}

	std::shared_ptr<Texture> Texture::Resolve(Graphics& gfx, const std::string& path, UINT slot)
	{
		return Codex::Resolve<Texture>(gfx, path, slot);

	}

	std::string Texture::GenerateUID(const std::string& path, UINT slot)
	{
		using namespace std::string_literals;
		return typeid(Texture).name() + "#"s + path + "#"s + std::to_string(slot);
	}
	std::string Texture::GetUID() const  noexcept
	{
		return GenerateUID(path, slot);
	}
}