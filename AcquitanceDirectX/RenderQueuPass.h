#pragma once
#include "BindingPass.h"
#include "Job.h"

class RenderQueuePass : public BindingPass
{
public:
	RenderQueuePass(const std::string& name)
		:
		BindingPass(name)
	{}
	void Accept(Job job_in)
	{
		jobs.push_back(std::move(job_in));
	}

	void Reset() override
	{
		jobs.clear();
	}

	virtual void Execute(Graphics& gfx) override
	{
		BindPassingResources(gfx);
		BindAll(gfx);
		for (auto& j : jobs)
		{
			j.Execute(gfx);
		}
	}

private:
	std::vector<Job> jobs;
};