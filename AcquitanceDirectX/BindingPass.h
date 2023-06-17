#pragma once
#include <memory>
#include "Pass.h"

namespace Bind
{
	class Bindable;
};

class RenderTarget;
class DepthStencil;

class BindingPass : public Pass
{
public:
	BindingPass(const std::string& name)
		:
		Pass(name)
	{}

	void AddBind(std::shared_ptr<Bind::Bindable> bindable_in);
	void BindAll(Graphics& gfx);
	
	void BindPassingResources(Graphics& gfx);
	template<class T>
	void AddBindSink(std::string name)
	{
		const auto index = bindables.size();
		bindables.emplace_back();
		RegisterSink(std::make_unique<ContainerBindableSink<T>>(std::move(name), bindables, index));
	}
	
protected:
	std::shared_ptr<RenderTarget> target;
	std::shared_ptr<DepthStencil> stencil;

	std::vector<std::shared_ptr<Bind::Bindable>> bindables;
};