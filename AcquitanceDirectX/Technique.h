#pragma once
#include <vector>
#include "Step.h"
class Technique
{
public:
	void Activate() noexcept
	{
		active = true;
	}

	void Deactivate() noexcept
	{
		active = false;
	}

	void AddStep(Step step)
	{
		steps.push_back(std::move(step));
	}

	void Submit(class FrameComander& frame, const  class Drawable& drawable) const;
	

	void InitializeParentReference(const class Drawable& drawable) noexcept;
	

private:
	bool active = true;
	std::vector<Step> steps;
};