#pragma once
#include "GraphicsResourse.h"
#include "GraphicsThrowHeader.h"
#include "Bindable.h"
#include "BufferResource.h"




class DepthStencil :public Bind::Bindable, public BufferResource
{
    friend class RenderTarget;
    friend class Graphics;
public :
    DepthStencil(Graphics& gfx, UINT width, UINT height);
	
    void Bind(Graphics& gfx) noexcept override;
  
    void Clear(Graphics& gfx) const noexcept override;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> getComPtr()const
    {
        return pDSView;
    }
private:
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDSView;
};