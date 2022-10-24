#pragma once
#include "DrawableBase.h"
#include "TestObj.h"
class ConeDrawable : public TestObj<ConeDrawable>
{
public:
	ConeDrawable(Graphics& gfx, std::mt19937& rng,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist,
		std::uniform_real_distribution<float>& bdist,
		int tessalation);
	DirectX::XMMATRIX GetTransformXM() const noexcept override;

private:

	// model transform
	DirectX::XMFLOAT3X3 mt;
};
