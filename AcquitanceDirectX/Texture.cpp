#include "Texture.h"
#include "GraphicsThrowHeader.h"
namespace Bind {
	Texture::Texture(Graphics& gfx, const Surface& surf)
	{
		INFOMAN(gfx);

		Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;

		D3D11_TEXTURE2D_DESC tex_des = {};
		tex_des.Width = surf.GetWidth();
		tex_des.Height = surf.GetHeight();
		tex_des.MipLevels = 1u;
		tex_des.ArraySize = 1u;
		tex_des.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		tex_des.Usage = D3D11_USAGE_DEFAULT;
		tex_des.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		tex_des.CPUAccessFlags = 0u;
		tex_des.MiscFlags = 0u;
		tex_des.SampleDesc.Count = 1u;
		tex_des.SampleDesc.Quality = 0u;

		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = surf.GetBufferPtr();
		sd.SysMemPitch = (surf.GetWidth() * sizeof(Surface::Color));

		GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(&tex_des, &sd, &pTexture));

		D3D11_SHADER_RESOURCE_VIEW_DESC res_des = {};
		res_des.Format = tex_des.Format;
		res_des.Texture2D.MostDetailedMip = 0u;
		res_des.Texture2D.MipLevels = 1u;
		res_des.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

		GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(pTexture.Get(), &res_des, &pTextureView));


	}

	void Texture::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->PSSetShaderResources(0u, 1u, pTextureView.GetAddressOf());
	}
}