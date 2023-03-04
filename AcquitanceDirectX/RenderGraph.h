#pragma once
#include <vector>
#include "BufferResource.h"
#include "LambertianPass.h"
#include "OutlineMaskPass.h"
#include "OutlineDrawPass.h"
#include "VerticalBlurrPass.h"
#include "ClearBufferPass.h"
#include "Source.h"

class RenderGraph
{
public :
	RenderGraph(Graphics& gfx)
		:
		backBufferTarget(gfx.GetTarget()),
		masterDepth(std::make_shared<DepthStencil>(gfx, gfx.GetWidth(),gfx.GetHeight())),
		blurMaster(std::make_shared<BlurManager>(gfx))
	{
		AddGlobalSource(BindPassSource<RenderTarget>::Make("bufferInp", backBufferTarget));
		AddGlobalSource(DirectPassSource<DepthStencil>::Make("stencil", masterDepth));
		AddGlobalSink(DirectPassSink<RenderTarget>::Make("bufferOut", backBufferTarget));
		AddGlobalSource(BindPassSource<BlurManager>::Make("blurPapa", blurMaster));
	}

	void SetGlobalComp(const std::string& sinkName, const std::string& target)
	{
		const auto finder = [&sinkName](const std::unique_ptr<Sink>& p) {
			return p->GetRegisteredName() == sinkName;
		};
		const auto i = std::find_if(globalSinks.begin(), globalSinks.end(), finder);
		if (i == globalSinks.end())
		{
			throw std::runtime_error("Global sink does not exist: " + sinkName);
		}
		auto targetSplit = SplitString(target, ".");
		if (targetSplit.size() != 2u)
		{
			throw std::runtime_error("Input target has incorrect format");
		}
		(*i)->SetPass(targetSplit[0]);
		(*i)->SetSource(targetSplit[1]);
	}

	Source& GetGlobalSource(const std::string& source_name)
	{
		for (auto& s : globalSources)
		{
			if (s->GetRegisteredName() == source_name)
			{
				return *s;
			}
		}
		//throw std::runtime_error("no such source");
	}

	void BindGlobalComps() 
	{
		for (auto& si : globalSinks)
		{
			using namespace std::string_literals;
			for (const auto& pass : passes)
			{
				for (auto& so : pass->GetSources())
				{
					if (so->GetRegisteredName() == si->GetSourceName())
					{
						si->Bind(so);
					}
				}
			}
		}
	}

	void SetSinkLinkage(Pass& pass_in)
	{
		for (auto& si : pass_in.GetSinks())
		{
			for (auto& res : globalSources)
			{
				using namespace std::string_literals;
				if (si->GetSourceName() == "$"s)
				{
					for (auto& gsource : globalSources)
					{
						if (gsource->GetRegisteredName() == si->GetSourceName())
						{
							si->Bind(gsource);
						}
					}
				}
				else
				{
					for (const auto& pass : passes)
					{
						for (auto& so : pass->GetSources())
						{
							if (so->GetRegisteredName() == si->GetSourceName())
							{
								si->Bind(so);
							}
						}
					}
				}
			}
		}
	}

	void Execute(Graphics& gfx) 
	{
		for (auto& p : passes)
		{
			p->Execute(gfx);
		}
	}

	void Reset() 
	{
		for (auto& p : passes)
		{
			p->Reset();
		}
	}

	void AddPass(std::unique_ptr<Pass> pass_in)
	{
		SetSinkLinkage(*pass_in);
		passes.push_back(std::move(pass_in));
	}

	void AddGlobalSink(std::unique_ptr<Sink> sink_in)
	{
		globalSinks.push_back(std::move(sink_in));
	}

	void AddGlobalSource(std::unique_ptr < Source> source_in)
	{
		globalSources.push_back(std::move(source_in));
	}

	Pass& findPass(const std::string& name_in)
	{
		auto result = std::find_if(passes.begin(), passes.end(), [name_in](const std::unique_ptr<Pass>& pName) {
			return name_in == pName->GetName();
		});
		if (result == std::end(passes))
		{
			throw std::runtime_error("the pass is not present in the vector");
		}
		return **result;

	}

	RenderQueuePass& GetRenderQueue(const std::string name) 
	{
		try {
			for (auto& pass : passes)
			{
				if (pass->GetName() == name)
				{
					RenderQueuePass& r1 = dynamic_cast<RenderQueuePass&>(*pass);
					return r1;
				}
			}
		}
		catch(const std::bad_cast& e)
		{
			std::runtime_error(e.what());
		}
	}

protected:
	std::shared_ptr<RenderTarget> backBufferTarget;
	std::shared_ptr<DepthStencil> masterDepth;
	std::shared_ptr<BlurManager> blurMaster;
	

private:
	std::vector<std::unique_ptr<Sink>> globalSinks;
	std::vector<std::unique_ptr<Source>> globalSources;
	std::vector<std::unique_ptr<Pass>> passes;
};



class OutlineRenderGraph : public RenderGraph
{
public:
	OutlineRenderGraph(Graphics& gfx)
		:
		RenderGraph(gfx)
	{
		{
			auto pass = std::make_unique<ClearBufferPass>("clearRt");
			pass->SetPassLinkage("buffer", "$.bufferInp");
			AddPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<ClearBufferPass>("clearDS");
			pass->SetPassLinkage("buffer", "$.stencil");
			AddPass(std::move(pass));
		}
		{
			auto pass = std::make_unique < LambertianPass>("Lambertian", gfx);
			pass->SetPassLinkage("buffer", "clearRt.buffer");
			pass->SetPassLinkage("depthStencil", "clearDS.buffer");
			AddPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<OutlineMaskPass>("outlineMask",gfx);
			pass->SetPassLinkage("depthStencil", "Lambertian.depthStencil");
			AddPass(std::move(pass));
		}

		{
			auto pass = std::make_unique<OutlineDrawPass>("outlineDraw", gfx);
			AddPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<FullScreenPass>("horizontal", gfx);
			pass->SetPassLinkage("scratch_in", "outlineDraw.buffer");
			pass->SetPassLinkage("blur", "$.blurPapa");
			AddPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<VerticalBlurrPass>("vertical", gfx);
			pass->SetPassLinkage("scratch_in", "horisontal.scratch_out");
			pass->SetPassLinkage("buffer", "Lambertian.buffer");
			pass->SetPassLinkage("depthStencil", "outlineMask.depthStencil");
			blurMaster->SetVertical(gfx);
			pass->SetPassLinkage("blur", "&.blurPapa");
		}
		SetGlobalComp("bufferOut", "vertical.buffer");
		BindGlobalComps();
	}
};