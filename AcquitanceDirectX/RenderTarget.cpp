#include "RenderTarget.h"
#include "Surface.h"
RenderTarget::RenderTarget(Graphics& gfx, UINT width, UINT height, UINT slot )
	:
	width(width),
	height(height),
	slot(slot)
{
	INFOMAN(gfx);

	// create texture resource
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
	GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(
		&textureDesc, nullptr, &pTexture
	));

	// create the resource view on the texture
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(
		pTexture.Get(), &srvDesc, &pResourceView
	));

	// create the target view on the texture
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = textureDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };
	GFX_THROW_INFO(GetDevice(gfx)->CreateRenderTargetView(
		pTexture.Get(), &rtvDesc, &pTargetView
	));
}

void RenderTarget::BindAsTexture(Graphics& gfx, UINT slot) const noexcept
{
	GetContext(gfx)->PSSetShaderResources(slot, 1, pResourceView.GetAddressOf());
}

Surface RenderTarget::ToSurface(Graphics& gfx) const
{
	namespace wrl = Microsoft::WRL;

	// creating a temp texture compatible with the source, but with CPU read access
	wrl::ComPtr<ID3D11Resource> pResSource;
	pTargetView->GetResource(&pResSource);
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
		auto pSrcRow = reinterpret_cast<const Surface::Color*>(pSrcBytes + msr.RowPitch * size_t(y));
		for (unsigned int x = 0; x < width; x++)
		{
			s.PutPixel(x, y, *(pSrcRow + x));
		}
	}
	GetContext(gfx)->Unmap(pTexTemp.Get(), 0);

	return s;
}

void RenderTarget::Bind(Graphics& gfx) noexcept 
{
	GetContext(gfx)->PSSetShaderResources(slot, 1, pResourceView.GetAddressOf());
}


void RenderTarget::BindAsTarget(Graphics& gfx) const noexcept
{
	D3D11_VIEWPORT vp;
	vp.Width = width;
	vp.Height = height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	GetContext(gfx)->RSSetViewports(1u, &vp);
	GetContext(gfx)->OMSetRenderTargets(1, pTargetView.GetAddressOf(), nullptr);
}


void RenderTarget::BindAsTarget(Graphics& gfx, const DepthStencil& depthStencil) const noexcept
{
	D3D11_VIEWPORT vp;
	vp.Width = width;
	vp.Height = height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	GetContext(gfx)->RSSetViewports(1u, &vp);
	GetContext(gfx)->OMSetRenderTargets(1, pTargetView.GetAddressOf(), *depthStencil.getComPtr().GetAddressOf());
}

void RenderTarget::Clear(Graphics& gfx) const noexcept 
{
	const float color[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	GetContext(gfx)->ClearRenderTargetView(pTargetView.Get(), color);
}

void RenderTarget::Clear(Graphics& gfx, const float* pFirstArrEl) const noexcept
{
	GetContext(gfx)->ClearRenderTargetView(pTargetView.Get(), pFirstArrEl);
}

BackBufferRenderTraget::BackBufferRenderTraget(Graphics& gfx, Microsoft::WRL::ComPtr<ID3D11Resource> back_buff, UINT width, UINT height, UINT slot )
	:
	RenderTarget(gfx, width, height, slot)

{
	GetDevice(gfx)->CreateRenderTargetView(back_buff.Get(), nullptr, &pTargetView);
}