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


};