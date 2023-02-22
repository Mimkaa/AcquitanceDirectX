#pragma once
#include "BindableCommon.h"
#include "ChiliMath.h"
#include "Graphics.hpp"
#include "ImguiManager.h"
#include "imgui/imgui.h"

class BlurManager
{
public:
	BlurManager(Graphics& gfx, float radius = 7.0f, float sigma = 2.6f)
		:
		kcb{gfx,0},
		ccb{gfx,1},
		ps{gfx, "GaussBlur_PS.cso"},
		radius(radius),
		sigma(sigma)

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

	void ShowWindow(Graphics& gfx)
	{
		if (ImGui::Begin("BlurrControl"))
		{
			int rad = (int)radius;
			bool changeSig = ImGui::SliderInt("radius", &rad, 1, 7);
			bool changeRad = ImGui::SliderFloat("Sigma", &sigma, 1.0f, 10.0f, "%.1f", 1.5f);
			radius = (float)rad;
			if (changeRad || changeSig)
			{
				FillKernel(gfx, radius, sigma);

			}
		}

		ImGui::End();
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
	float radius;
	float sigma;
};