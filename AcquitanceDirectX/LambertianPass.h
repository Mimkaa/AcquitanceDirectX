#pragma once
#include "RenderQueuPass.h"
#include "Camera.h"
#include "ShadowCameraCbuf.h"

class LambertianPass : public RenderQueuePass
{
public:
	LambertianPass(const std::string& name, Graphics& gfx)
		:
		RenderQueuePass(name),
		pShadowCbuf{std::make_shared<Bind::ShadowCameraCbuf>(gfx)}
	{
		RegisterSink(DirectPassSink<RenderTarget>::Make("buffer", target));
		RegisterSource(DirectPassSource<RenderTarget>::Make("buffer", target));
		RegisterSink(DirectPassSink<DepthStencil>::Make("depthStencil", stencil));
		RegisterSource(DirectPassSource<DepthStencil>::Make("depthStencil", stencil));
		RegisterSink(DirectPassSink<Bind::Bindable>::Make("shadowMap", pDepthMap));
		AddBind(pShadowCbuf);
		AddBind(Bind::Blender::Resolve(gfx, false));
		AddBind(Bind::Stencil::Resolve(gfx, Bind::Stencil::Mode::Off));
	}
	void SetMainCamRef(const Camera& cam)
	{
		pMainCam = &cam;
	}

	void BindShadowCamera(const Camera& cam) noexcept
	{
		pShadowCbuf->SetCamera(cam);
	}

	void Execute(Graphics& gfx) override
	{
		assert(pMainCam);
		pShadowCbuf->Update(gfx);
		pMainCam->BindToGraphics(gfx);
		RenderQueuePass::Execute(gfx);
	}
	
	
private:
	const Camera* pMainCam = nullptr;
	std::shared_ptr<Bind::ShadowCameraCbuf> pShadowCbuf;
	std::shared_ptr<Bind::Bindable> pDepthMap;

};