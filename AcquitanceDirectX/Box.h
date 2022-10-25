#pragma once
#include "DrawableBase.h"
#include "TestObj.h"
#include "ConstantBuffers.h"
class Box : public TestObj<Box>
{
public:
	Box(Graphics& gfx, std::mt19937& rng,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist,
		std::uniform_real_distribution<float>& bdist,
		DirectX::XMFLOAT3 mat);
	DirectX::XMMATRIX GetTransformXM() const noexcept override;

	void SpawnControlWindow(int id, Graphics& gfx);
private:
	void SyncMaterial(Graphics& gfx)noexcept(!IS_DEBUG);

private:
	struct PSMaterialConstants
	{
		DirectX::XMFLOAT3 color;
		float specularIntensity = 0.6f;
		float specularPower = 30.0f;
		float padding[3];

	} materialConstants;
	using MaterialCBuf = PixelConstantBuffer<PSMaterialConstants>;
private:
	// model transform
	DirectX::XMFLOAT3X3 mt;
};