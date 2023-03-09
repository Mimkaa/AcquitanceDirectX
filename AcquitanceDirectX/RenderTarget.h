#pragma once
#include "GraphicsResourse.h"
#include "DepthStencil.h"
#include "Bindable.h"
#include "BufferResource.h"


class RenderTarget :public Bind::Bindable, public BufferResource
{
public:
	RenderTarget(Graphics& gfx, UINT width, UINT height, UINT slot = 0u);
	
	RenderTarget() = default;
	

	void BindAsTexture(Graphics& gfx, UINT slot) const noexcept;

	void Bind(Graphics& gfx) noexcept override;


	void BindAsTarget(Graphics& gfx) const noexcept;


	void BindAsTarget(Graphics& gfx, const DepthStencil& depthStencil) const noexcept;

	void Clear(Graphics& gfx) const noexcept override;

	void Clear(Graphics& gfx, const float* pFirstArrEl) const noexcept;

public:
	UINT GetWidth() const
	{
		return width;
	}
	UINT GetHeight() const
	{
		return height;
	}

protected:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pResourceView;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTargetView;
	UINT width;
	UINT height;
	UINT slot;
};


class BackBufferRenderTraget : public RenderTarget
{
public:
	BackBufferRenderTraget(Graphics& gfx, Microsoft::WRL::ComPtr<ID3D11Resource> back_buff, UINT width, UINT height, UINT slot = 0u);
	
};