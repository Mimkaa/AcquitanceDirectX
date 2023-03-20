#pragma once
#include "FullScreenPass.h"

class VerticalBlurrPass : public FullScreenPass
{
public:
	VerticalBlurrPass(const std::string& name, Graphics& gfx)
		:
		FullScreenPass(name, gfx)
	{
		RegisterSink(DirectPassSink<DepthStencil>::Make("depthStencil", stencil));
		RegisterSource(DirectPassSource<DepthStencil>::Make("depthStencil", stencil));
		RegisterSink(DirectPassSink<RenderTarget>::Make("buffer", target));
		RegisterSource(DirectPassSource<RenderTarget>::Make("buffer", target));
		AddBind(Bind::Stencil::Resolve(gfx, Bind::Stencil::Mode::Mask));

	}
	void Execute(Graphics& gfx) override
	{
		BindPassingResources(gfx);
		rt->BindAsTexture(gfx, 0u);
		//target->ToSurface(gfx).Save("screen.png");
		BindAll(gfx);
		bl->Bind(gfx);
		gfx.DrawIndexed(FullIb->GetCount());
		
	}
};