#pragma once
#include "RenderQueuPass.h"
#include <string> 
#include "BindableCommon.h"

class WireFramePass :public RenderQueuePass
{
public:
	WireFramePass(Graphics& gfx, const std::string& name)
		:
		RenderQueuePass( name )
	{
		RegisterSink(DirectPassSink<DepthStencil>::Make("depthStencil", stencil));
		RegisterSource(DirectPassSource<DepthStencil>::Make("depthStencil", stencil));
		RegisterSink(DirectPassSink<RenderTarget>::Make("buffer", target));
		RegisterSource(DirectPassSource<RenderTarget>::Make("buffer", target));
		AddBind(Bind::Stencil::Resolve(gfx, Bind::Stencil::Mode::ReverseSample));
	}
};