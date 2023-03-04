#pragma once
#include "RenderQueuPass.h"

class OutlineMaskPass :public RenderQueuePass
{
public:
	OutlineMaskPass(const std::string& name, Graphics& gfx)
		:
		RenderQueuePass(name)
	{
		RegisterSink(DirectPassSink<DepthStencil>::Make("depthStencil", stencil));
		RegisterSource(DirectPassSource<DepthStencil>::Make("depthStencil", stencil));

		AddBind(Bind::Stencil::Resolve(gfx, Bind::Stencil::Mode::Write));
		AddBind(Bind::NullPixelShader::Resolve(gfx));
	}

};