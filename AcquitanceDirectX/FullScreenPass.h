#pragma once
#include "BindingPass.h"
#include <memory>


class Bindable;

class FullScreenPass :public BindingPass
{
public:
	FullScreenPass(const std::string& name, Graphics& gfx)
		:
		BindingPass(name)
	{
		RegisterSource(BindPassSource<RenderTarget>::Make("scratch_in", rt1));
		RegisterSink(DirectPassSink<RenderTarget>::Make("scratch_out", rt));

		RegisterSink(BindPassSink<BlurManager>::Make("blur", bl));
	
		Dvtx::VertexLayout vl{};
		vl.Append(Dvtx::VertexLayout::Position2D);
		Dvtx::VertexBuffer vb{ vl };
		vb.EmplaceBack(DirectX::XMFLOAT2{ -1,1 });
		vb.EmplaceBack(DirectX::XMFLOAT2{ 1,1 });
		vb.EmplaceBack(DirectX::XMFLOAT2{ 1,-1 });
		vb.EmplaceBack(DirectX::XMFLOAT2{ -1,-1 });

		std::vector<unsigned short> inds = { 0,1,2,2,3,0 };

		auto FullVS = Bind::VertexShader::Resolve(gfx, "MapNDStoSurface_VS.cso");
	
		AddBind(Bind::VertexBuffer::Resolve(gfx, "$FullScreenFilterVB", std::move(vb)));

		FullIb = Bind::IndexBuffer::Resolve(gfx, "$FullScreenFilterIB", std::move(inds));

		AddBind(FullIb);
		AddBind(Bind::InputLayout::Resolve(gfx, vb.GetLayout(), FullVS->GetBytecode()));
		AddBind(std::move(FullVS));
		AddBind(Bind::PixelShader::Resolve(gfx, "GaussBlur_PS.cso"));
		AddBind(Bind::Blender::Resolve(gfx, true));
		AddBind(Bind::Sampler::Resolve(gfx, true, false));
		AddBind(Bind::Topology::Resolve(gfx));
		


	}
	void PerformBlur(Graphics& gfx)
	{
		BindAll(gfx);
		rt1->BindAsTarget(gfx);
		rt->BindAsTexture(gfx, 0u);
		bl->Bind(gfx);
		gfx.DrawIndexed(FullIb->GetCount());
	}
	virtual void Execute(Graphics& gfx) override
	{
		PerformBlur(gfx);

	}
	void SetAdditionalBuffer(Graphics& gfx, int width, int height)
	{
		rt1 = std::make_shared<RenderTarget>(gfx,width, height);
	}
protected:
	std::shared_ptr<RenderTarget> rt;
	std::shared_ptr<RenderTarget> rt1;
	std::shared_ptr<BlurManager> bl;
	std::shared_ptr<Bind::IndexBuffer> FullIb;
};