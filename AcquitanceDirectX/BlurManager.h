#pragma once
#include "BindableCommon.h"
#include "ChiliMath.h"
#include "Graphics.hpp"
class BlurManager
{
public:
	BlurManager(Graphics& gfx, float radius = 7.0f, float sigma = 2.6f)
		:
		kcb{gfx,0},
		ccb{gfx,1},
		ps{gfx, "GaussBlur_PS.cso"}

	{
		FillKernel(gfx, radius, sigma);
	}
	void Bind(Graphics& gfx)
	{
		ps.Bind(gfx);
		kcb.Bind(gfx);
		ccb.Bind(gfx);

	}
	void FillKernel(Graphics& gfx, float radius, float sigma)
	{
		assert(radius <= 7.0f);
		Kernel kerl;
		float diameter =  (radius * 2) + 1;
		kerl.Taps = diameter;
		float sum = 0.0f;
		for (int i = 0; i < diameter; i++)
		{
			auto val = gauss(i - radius, sigma);
			sum += val;
			kerl.coefficients[i].x = val;
		}
		for (int i = 0; i < diameter; i++)
		{
			kerl.coefficients[i].x /= sum;
		}
		kcb.Update(gfx, kerl);

	}
	void SetHorizontal(Graphics& gfx)
	{
		ccb.Update(gfx, { TRUE });
	}

	void SetVertical(Graphics& gfx)
	{
		ccb.Update(gfx, { FALSE });
	}

private:
	struct Kernel
	{
		UINT Taps;
		float padding[3];
		DirectX::XMFLOAT4 coefficients[15];
	};
	struct Control
	{
		BOOL horizontal;
		float padding[3];
	};
	Bind::PixelConstantBuffer<Control> ccb;
	Bind::PixelConstantBuffer<Kernel>kcb;
	Bind::PixelShader ps;
};