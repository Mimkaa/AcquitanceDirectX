#include "Step.h"
#include "FrameCommander.h"
#include "Drawable.h"

void Step::AddBindable(std::shared_ptr<Bind::Bindable> bindable_in)
{
	bindables.push_back(std::move(bindable_in));
}

void Step::Bind(Graphics& gfx) const noexcept
{
	for (auto& b : bindables)
	{
		b->Bind(gfx);
	}
}

void Step::Submit(FrameComander& frame, const Drawable& drawable) const noexcept
{
	frame.Accept(Job{ &drawable, this }, index);
}

void Step::InitializeParentReference(const Drawable& drawable) noexcept
{
	for (auto& b : bindables)
	{
		b->InitializeParentReference(drawable);
	}
}
