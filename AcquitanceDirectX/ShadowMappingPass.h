#pragma once
#include "RenderQueuPass.h"
#include <memory>
#include "BindableCommon.h"
#include "Camera.h"
#include "DepthStencil.h"
#include "Vertex.h"
class ShadowMappingPass : public RenderQueuePass
{
public:
	void SetShadowCamRef(const Camera& cam) noexcept
	{
		pShadowCamera = &cam;
	}
	ShadowMappingPass(Graphics& gfx, std::string name)
		:
		RenderQueuePass(std::move(name))
	{
		using namespace Bind;
		
		
		stencil = std::make_shared<ShaderInputDepthStencil>(gfx, 3, DepthStencil::Usage::ShadowDepthUsage);
		AddBind(VertexShader::Resolve(gfx, "Solid_VS.cso"));
		AddBind(NullPixelShader::Resolve(gfx));    
		// must not be a problem
		AddBind(Stencil::Resolve(gfx, Stencil::Mode::Off));
		AddBind(Blender::Resolve(gfx, false));
		
		

		
		
		RegisterSource(DirectPassSource<DepthStencil>::Make("shadowMap", stencil));
	}
	
	void Execute(Graphics& gfx)  noexcept override
	{
		// it makes jobs for each step and its bindables so it was not the problem
		// also it has all the bindables we added
		stencil->Clear(gfx);
 		pShadowCamera->BindToGraphics(gfx);
		RenderQueuePass::Execute(gfx);
	}
	void DumpCamera(Graphics& gfx, const std::string& path) const
	{
		
		stencil->ToSurface(gfx).Save(path);
	}
private:
	const Camera* pShadowCamera = nullptr;
	
};