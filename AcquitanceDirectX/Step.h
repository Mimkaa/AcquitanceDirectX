#pragma once
#include <vector>
#include <memory>
#include "Bindable.h"
#include "Graphics.hpp"




class Step
{
public:

	Step(size_t passIndex)
		:
		index(passIndex)
		{}

	void AddBindable(std::shared_ptr<Bind::Bindable> bindable_in);

	void Bind(Graphics& gfx) const noexcept;

	void Submit(class FrameComander& frame, const class Drawable& drawable) const noexcept;
	

	void InitializeParentReference(const class Drawable& drawable) noexcept;
	

private:
	const size_t index;
	std::vector<std::shared_ptr<Bind::Bindable>> bindables;
};