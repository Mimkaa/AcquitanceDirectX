#pragma once
#include "GraphicsResourse.h"
#include "GraphicsThrowHeader.h"
#include "Bindable.h"
#include "BufferResource.h"

class Surface;



class DepthStencil :public Bind::Bindable, public BufferResource
{
    friend class RenderTarget;
    friend class Graphics;
public :
    enum Usage
    {
        DepthStencilUsage,
        ShadowDepthUsage
    };

    Surface ToSurface(Graphics& gfx, bool lineralze = false) const;
    DepthStencil(Graphics& gfx, UINT width, UINT height, Usage usage = Usage::DepthStencilUsage);
	
    void Bind(Graphics& gfx) noexcept override;
  
    void Clear(Graphics& gfx) const noexcept override;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> getComPtr()const
    {
        return pDSView;
    }

    unsigned int GetWidth() const;
    unsigned int GetHeight() const;

private:
    int width;
    int height;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDSView;
};