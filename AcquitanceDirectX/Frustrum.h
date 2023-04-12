#pragma once
#include "Drawable.h"

class Graphics;

class Frustrum :public Drawable
{
public:
	Frustrum(Graphics& gfx, float width, float height, float fFar, float fNear) noexcept;
	void SetPosition(DirectX::XMFLOAT3 pos) noexcept;
	void SetRotation(DirectX::XMFLOAT3 rot) noexcept;
	void SetVertBuffer(Graphics& gfx, float width, float height, float fFar, float fNear);
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	DirectX::XMFLOAT3 pos = {0.0f, 0.0f, 0.0f};
	DirectX::XMFLOAT3 rot = { 0.0f, 0.0f, 0.0f };
};