#pragma once
#include "Drawable.h"
#include "ConstantBuffers.h"
class TestCube :public Drawable
{
public:
	TestCube(Graphics& gfx);
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void ShowControlWindow(Graphics& gfx) noexcept;
private:
	struct
	{
		DirectX::XMFLOAT3 pos = { -1,1, -1 };
		float roll = 0.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;

	}pos;

	struct PSConstant
	{
		float specularIntensity = 1.0f;
		float specularPower = 20;
		BOOL normalsOn = TRUE;
		float padding[1];

	}Const;


};