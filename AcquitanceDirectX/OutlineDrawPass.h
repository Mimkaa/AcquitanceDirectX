#pragma once
#include "RenderQueuPass.h"

class OutlineDrawPass : public RenderQueuePass
{
public:
	OutlineDrawPass(const std::string& name, Graphics& gfx)
		:
		RenderQueuePass(name)
	{

		RegisterSource(BindPassSource<RenderTarget>::Make("buffer", target));

	}
	void SetTargetAndStencil(Graphics& gfx, int width, int height)
	{
		target =  std::make_shared<RenderTarget>(gfx, width, height);
		stencil = std::make_shared<DepthStencil>(gfx, width, height);
	}
	void ClearBuffers(Graphics& gfx)
	{
		target->Clear(gfx);
		stencil->Clear(gfx);
	}


};