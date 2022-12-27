#pragma once
#include "Drawable.h"
#include "ConstantBuffers.h"
class TestPlane :public Drawable
{
public:
	TestPlane(Graphics& gfx, DirectX::XMFLOAT4 color_in ={1.0f,1.0f,1.0f,0.0f});
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void SetPos(const DirectX::XMMATRIX mat) noexcept;
	void SetPos(const DirectX::XMVECTOR vec) noexcept;
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
		DirectX::XMFLOAT4 color;
		
	} Const;

	
};