#pragma once
#include "BindableCommon.h"
#include "ChiliMath.h"
#include "Graphics.hpp"
#include "ImguiManager.h"
#include "imgui/imgui.h"

class BlurManager
{
public:
	BlurManager(Graphics& gfx, float radius = 7.0f, float sigma = 2.6f, const char* PsType = "GaussBlurOutline_PS.cso")
		:
		kcb{gfx,0},
		ccb{gfx,1},
		ps{gfx, PsType},
		radius(radius),
		sigma(sigma)

	{
		FillKernelGauss(gfx, radius, sigma);
	}
	void Bind(Graphics& gfx)
	{
		ps.Bind(gfx);
		kcb.Bind(gfx);
		ccb.Bind(gfx);

	}
	void FillKernelGauss(Graphics& gfx, float radius, float sigma)
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

	void FillKernelBox(Graphics& gfx, float radius)
	{
		assert(radius <= 7.0f);
		Kernel kerl;
		float diameter = (radius * 2) + 1;
		kerl.Taps = diameter;
		float factor = 1.0f / diameter;
		for (int i = 0; i < diameter; i++)
		{
			kerl.coefficients[i].x = factor;
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

	void ShowWidgets(Graphics& gfx)
	{
		
			bool filterChanged = false;
			{
				const char* items[] = { "Gauss","Box" };
				static const char* curItem = items[0];
				if (ImGui::BeginCombo("Filter Type", curItem))
				{
					for (int n = 0; n < std::size(items); n++)
					{
						const bool isSelected = (curItem == items[n]);
						if (ImGui::Selectable(items[n], isSelected))
						{
							filterChanged = true;
							curItem = items[n];
							if (curItem == items[0])
							{
								kernelType = KernelType::Gauss;
							}
							else if (curItem == items[1])
							{
								kernelType = KernelType::Box;
							}
						}
						if (isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
			}
			int rad = (int)radius;
			bool changeSig = ImGui::SliderInt("radius", &rad, 1, 7);
			bool changeRad = ImGui::SliderFloat("Sigma", &sigma, 1.0f, 10.0f, "%.1f", 1.5f);
			radius = (float)rad;
			if (changeRad || changeSig || filterChanged)
			{
				if (kernelType == KernelType::Gauss) {
					FillKernelGauss(gfx, radius, sigma);
				}
				else
				{
					FillKernelBox(gfx, radius);
				}
			}
		

		
	}

private:
	enum class KernelType
	{
		Gauss,
		Box,
	};
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
	KernelType kernelType = KernelType::Gauss;
	float radius;
	float sigma;
};