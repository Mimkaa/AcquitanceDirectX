#include "BindingPass.h"
#include "RenderTarget.h"
#include "DepthStencil.h"



void BindingPass::AddBind(std::shared_ptr<Bind::Bindable> bindable_in)
{
	bindables.push_back(std::move(bindable_in));
}

void BindingPass::BindAll(Graphics& gfx)
{

	for (const auto& b : bindables)
	{
		b->Bind(gfx);
	}
}

void BindingPass::BindPassingResources(Graphics& gfx)
{
	if (target)
	{
		target->BindAsTarget(gfx, *stencil);
	}
	else
	{
		stencil->Bind(gfx);
	}
}