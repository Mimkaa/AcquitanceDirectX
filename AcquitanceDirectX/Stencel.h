#pragma once
#include "Bindable.h"
namespace Bind {
	class Stencil :public Bindable
	{
	public:
		enum Style
		{
			Off,
			Write,
			Mask,
		};
		Stencil(Graphics& gfx, Style style_in)
		{
			D3D11_DEPTH_STENCIL_DESC dsDesc = CD3D11_DEPTH_STENCIL_DESC{ CD3D11_DEFAULT{} };
			
			if (style_in == Style::Write)
			{
				dsDesc.StencilEnable = true;
				dsDesc.StencilWriteMask = 0xFF;
				dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
				dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
			}
			if (style_in == Style::Mask) {
				dsDesc.StencilEnable = true;
				dsDesc.DepthEnable = false;
				dsDesc.StencilReadMask = 0xff;
				dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;
				dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			}
				
			
			GetDevice(gfx)->CreateDepthStencilState(&dsDesc, &pStencil);
		}

		void Bind(Graphics& gfx) noexcept
		{
			GetContext(gfx)->OMSetDepthStencilState(pStencil.Get(), 0xFF);
		}
	private:
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pStencil;
	};
}