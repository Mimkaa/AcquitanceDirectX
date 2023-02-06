#pragma once
#include <array>
#include "BindableCommon.h"
#include "Graphics.hpp"
#include "Job.h"
#include "Pass.h"



class FrameComander
{
public:
	void Accept(Job job, size_t target)
	{
		passes[target].Accept(job);
	}

	void Reset() noexcept
	{
		for (auto& p : passes)
		{
			p.Reset();
		}
	}

	void Execute(Graphics& gfx) const
	{
	
		Bind::Stencil::Resolve(gfx, Bind::Stencil::Mode::Off)->Bind(gfx);
		passes[0].Execute(gfx);

		Bind::Stencil::Resolve(gfx, Bind::Stencil::Mode::Write)->Bind(gfx);
		Bind::NullPixelShader::Resolve(gfx)->Bind(gfx);
		passes[1].Execute(gfx);

		Bind::Stencil::Resolve(gfx, Bind::Stencil::Mode::Mask)->Bind(gfx);
		passes[2].Execute(gfx);
		
	}
private:
	std::array<Pass,3> passes;
};