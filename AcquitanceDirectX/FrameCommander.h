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

class FrameComander
{
public:
	FrameComander(Graphics& gfx, int downsize = 2)
		:
		ds(gfx, gfx.GetWidth(), gfx.GetHeight()),
		downsize(downsize)
	{
		rt = RenderTarget{ gfx, gfx.GetWidth() / 2, gfx.GetHeight() / 2 };
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
		blur =  std::make_shared<BlurManager>(gfx);
		
	}

	void Accept(Job job, size_t target)
	{
		passes[target].Accept(job);
	}

	void Reset() noexcept
	{
		for (auto& p : passes)
		{
			p.Reset();
		}
	}
	void ShowWindows(Graphics& gfx)
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
	}

	void Execute(Graphics& gfx) const
	{
		// setup render target used for all passes
		ds.Clear(gfx);
		rt->Clear(gfx);
		rt1->Clear(gfx);

		gfx.BindSwapBuffer(ds);
		Bind::Blender::Resolve(gfx, false)->Bind(gfx);
		Bind::Stencil::Resolve(gfx, Bind::Stencil::Mode::Off)->Bind(gfx);
		passes[0].Execute(gfx);

		
		Bind::Stencil::Resolve(gfx, Bind::Stencil::Mode::Write)->Bind(gfx);
		Bind::NullPixelShader::Resolve(gfx)->Bind(gfx);
		passes[1].Execute(gfx);

		rt1->BindAsTarget(gfx);
		Bind::Stencil::Resolve(gfx, Bind::Stencil::Mode::Off)->Bind(gfx);
		passes[2].Execute(gfx);

		rt->BindAsTarget(gfx);
		rt1->BindAsTexture(gfx, 0);
		FullVb->Bind(gfx);
		FullIb->Bind(gfx);
		FullVS->Bind(gfx);
		FullInputLay->Bind(gfx);
		FullSamplePoint->Bind(gfx);
		FullBlander->Bind(gfx);
		blur->Bind(gfx);
		blur->SetHorizontal(gfx);
		gfx.DrawIndexed(FullIb->GetCount());


		

		gfx.BindSwapBuffer(ds);
		rt->BindAsTexture(gfx, 0);
		FullSampleAni->Bind(gfx);
		blur->SetVertical(gfx);
		Bind::Stencil::Resolve(gfx, Bind::Stencil::Mode::Mask)->Bind(gfx);
		gfx.DrawIndexed(FullIb->GetCount());
		
		
		
	}
	std::shared_ptr<BlurManager> GetBlurManager() const
	{
		return std::move(blur);
	}
private:
	std::array<Pass,3> passes;
	DepthStencil ds;
	std::optional<RenderTarget> rt;
	std::optional<RenderTarget> rt1;
	int downsize;

	std::shared_ptr<Bind::VertexBuffer> FullVb;
	std::shared_ptr<Bind::IndexBuffer> FullIb;
	std::shared_ptr<Bind::VertexShader> FullVS;
	std::shared_ptr<Bind::PixelShader> FullPS;
	std::shared_ptr<Bind::Topology> FullTopology;
	std::shared_ptr<Bind::InputLayout> FullInputLay;
	std::shared_ptr<Bind::Sampler> FullSamplePoint;
	std::shared_ptr<Bind::Sampler> FullSampleAni;
	std::shared_ptr<Bind::Blender> FullBlander;
	std::shared_ptr<BlurManager> blur;
};