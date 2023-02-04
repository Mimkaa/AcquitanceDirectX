#pragma once
#include <vector>
#include "Job.h"
#include "Graphics.hpp"

class Pass
{
public:
	void Execute(Graphics& gfx) const noexcept
	{
		for (auto j : jobs)
		{
			j.Execute(gfx);
		}
	}

	void Reset()
	{
		jobs.clear();
	}

	void Accept(Job job)
	{
		jobs.push_back(job);
	}

private:
	std::vector<Job> jobs;
};
