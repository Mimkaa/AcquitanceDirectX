#pragma once
#include <vector>
#include <memory>
#include "Bindable.h"
#include "Graphics.hpp"

#include "TechniqueProbe.h"

class RenderQueuePass;

class Step
{
public:

	Step(const std::string& passName)
		:
		passName(passName)
	{
		renderPass = nullptr;
	}

	Step(Step&&) = default;
	Step(const Step& src) noexcept
		:
		passName(src.passName),
		renderPass(src.renderPass)
	{
		bindables.reserve(src.bindables.size());
		for (auto& pb : src.bindables)
		{
			if (auto* pCloning = dynamic_cast<const Bind::CloningBindable*>(pb.get()))
			{
				bindables.push_back(pCloning->Clone());
			}
			else
			{
				bindables.push_back(pb);
			}
		}
	}
	Step& operator=(const Step&) = delete;
	Step& operator=(Step&&) = delete;

	void AddBindable(std::shared_ptr<Bind::Bindable> bindable_in);

	void Bind(Graphics& gfx) const noexcept;

	void Submit(class FrameComander& frame, const class Drawable& drawable)  noexcept;
	

	void InitializeParentReference(const class Drawable& drawable) noexcept;
	
	void Accept(TechniqueProbe& probe)
	{
		probe.SetStep(this);
		for (auto& s : bindables)
		{
			s->Accept(probe);
		}
	}
private:
	std::string passName;
	std::vector<std::shared_ptr<Bind::Bindable>> bindables;
	RenderQueuePass* renderPass;
};