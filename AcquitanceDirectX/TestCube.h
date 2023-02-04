#pragma once
#include "Drawable.h"
#include "ConstantBuffers.h"
#include <memory>
#include "Bindable.h"
#include "IndexBuffer.h"
class TestCube :public Drawable
{
public:
	bool outline = false;
	TestCube(Graphics& gfx);
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void ShowControlWindow(Graphics& gfx) noexcept;

	

private:
	std::vector<std::shared_ptr<Bind::Bindable>> outlineBinds;
	struct
	{
		DirectX::XMFLOAT3 pos = { 0,15, 5 };
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