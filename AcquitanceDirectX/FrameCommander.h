#pragma once
#include <array>
#include "BindableCommon.h"
#include "Graphics.hpp"
#include "Job.h"
#include "Pass.h"
#include "DepthStencil.h"
#include "RenderTarget.h"
#include "Vertex.h"
#include "BindableCommon.h"
#include "BlurManager.h"
#include "RenderGraph.h"

class FrameComander
{
public:
	FrameComander(Graphics& gfx, int downsize = 2)
		:
		org(gfx),
		downsize(downsize)
	{
		/*rt = RenderTarget{ gfx, gfx.GetWidth() / 2, gfx.GetHeight() / 2 };
		rt1 = RenderTarget(gfx, gfx.GetWidth() / 2, gfx.GetHeight() / 2);
		Dvtx::VertexLayout vl{};
		vl.Append(Dvtx::VertexLayout::Position2D);
		Dvtx::VertexBuffer vb{ vl };
		vb.EmplaceBack(DirectX::XMFLOAT2{ -1,1 });
		vb.EmplaceBack(DirectX::XMFLOAT2{ 1,1 });
		vb.EmplaceBack(DirectX::XMFLOAT2{ 1,-1 });
		vb.EmplaceBack(DirectX::XMFLOAT2{ -1,-1 });

		std::vector<unsigned short> inds = { 0,1,2,2,3,0 };
		FullVb = Bind::VertexBuffer::Resolve(gfx, "$FullScreenFilterVB", std::move(vb));
		FullIb = Bind::IndexBuffer::Resolve(gfx, "$FullScreenFilterIB", std::move(inds));
		FullVS = Bind::VertexShader::Resolve(gfx, "MapNDStoSurface_VS.cso");
		FullPS = Bind::PixelShader::Resolve(gfx, "GaussBlur_PS.cso");
		FullInputLay = Bind::InputLayout::Resolve(gfx, vl, FullVS->GetBytecode());
		FullTopology = Bind::Topology::Resolve(gfx);
		FullSamplePoint = Bind::Sampler::Resolve(gfx, false, false);
		FullSampleAni = Bind::Sampler::Resolve(gfx, true, false);
		FullBlander = Bind::Blender::Resolve(gfx, true);
		blur =  std::make_shared<BlurManager>(gfx);*/
		
	}

	/*void Accept(Job job, size_t target)
	{
		passes[target].Accept(job);
	}*/

	/*void Reset() noexcept
	{
		for (auto& p : passes)
		{
			p.Reset();
		}
	}*/
	/*void ShowWindows(Graphics& gfx)
	{
		if (ImGui::Begin("BlurrControl"))
		{
			bool changeSize = ImGui::SliderInt("radius", &downsize, 1, 10);
			blur->ShowWidgets(gfx);
			if (changeSize)
			{
				rt = RenderTarget{ gfx, gfx.GetWidth() / downsize, gfx.GetHeight() / downsize };
				rt1 = RenderTarget(gfx, gfx.GetWidth() / downsize, gfx.GetHeight() / downsize);
			}
		}

		ImGui::End();
	}*/

	void Execute(Graphics& gfx) 
	{
		org.Execute(gfx);
	}
	void Reset() 
	{
		org.Reset();
	}
	//std::shared_ptr<BlurManager> GetBlurManager() const
	//{
	//	return std::move(blur);
	//}
	RenderQueuePass& GetRenderQueue(const std::string name)
	{
		return org.GetRenderQueue(name);
	}

	void SaveDepth(Graphics& gfx, bool lineralze = false)
	{
		org.GetDepthStencil()->ToSurface(gfx, lineralze).Save("depth.png");
	}
	void BindMainCamera(Camera& cam)
	{
		org.BindMainCamera(cam);
	}
private:
	
	int downsize;
	OutlineRenderGraph org;
};