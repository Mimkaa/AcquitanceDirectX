#pragma once
#include "RenderQueuPass.h"
#include "Camera.h"

class LambertianPass : public RenderQueuePass
{
public:
	LambertianPass(const std::string& name, Graphics& gfx)
		:
		RenderQueuePass(name)
	{
		RegisterSink(DirectPassSink<RenderTarget>::Make("buffer", target));
		RegisterSource(DirectPassSource<RenderTarget>::Make("buffer", target));
		RegisterSink(DirectPassSink<DepthStencil>::Make("depthStencil", stencil));
		RegisterSource(DirectPassSource<DepthStencil>::Make("depthStencil", stencil));

		AddBind(Bind::Blender::Resolve(gfx, false));
		AddBind(Bind::Stencil::Resolve(gfx, Bind::Stencil::Mode::Off));
	}
	void SetMainCamRef(const Camera& cam)
	{
		pMainCam = &cam;
	}
	void Execute(Graphics& gfx) override
	{
		assert(pMainCam);
		pMainCam->BindToGraphics(gfx);
		RenderQueuePass::Execute(gfx);
	}
	
	
private:
	const Camera* pMainCam = nullptr;

};