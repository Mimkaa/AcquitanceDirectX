#pragma once
#include <vector>
#include "Step.h"
#include "TechniqueProbe.h"
class Technique
{
public:
	Technique() = default;

	Technique(const std::string& name)
		:
		name(name)
	{}

	void SetActiveState(bool state_in)
	{
		active = state_in;
	}

	void Activate() noexcept
	{
		active = true;
	}

	void Deactivate() noexcept
	{
		active = false;
	}

	bool IsActive() noexcept
	{
		return active;
	}

	void AddStep(Step step)
	{
		steps.push_back(std::move(step));
	}

	void Accept(TechniqueProbe& probe)
	{
		probe.SetTech(this);
		for (auto& s : steps)
		{
			s.Accept(probe);
		}
	}

	const std::string& GetName() const noexcept
	{
		return name;
	}

	void Submit(class FrameComander& frame, const  class Drawable& drawable) const;
	

	void InitializeParentReference(const class Drawable& drawable) noexcept;
	

private:
	bool active = true;
	std::vector<Step> steps;
	std::string name = "Nameless Tech";
};