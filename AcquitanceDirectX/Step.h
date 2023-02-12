#pragma once
#include <vector>
#include <memory>
#include "Bindable.h"
#include "Graphics.hpp"

#include "TechniqueProbe.h"


class Step
{
public:

	Step(size_t passIndex)
		:
		index(passIndex)
		{}
	Step(Step&&) = default;
	Step(const Step& src) noexcept
		:
		index(src.index)
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

	void Submit(class FrameComander& frame, const class Drawable& drawable) const noexcept;
	

	void InitializeParentReference(const class Drawable& drawable) noexcept;
	
	void Accept(TechniqueProbe& probe)
	{
		for (auto& s : bindables)
		{
			s->Accept(probe);
		}
	}
private:
	const size_t index;
	std::vector<std::shared_ptr<Bind::Bindable>> bindables;
};