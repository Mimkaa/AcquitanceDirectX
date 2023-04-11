#pragma once
#include "Drawable.h"
#include <DirectXMath.h>

class CameraIndicator:public Drawable
{
public:
	CameraIndicator(Graphics& gfx) noexcept;
	void SetPosition(DirectX::XMFLOAT3 pos) noexcept;
	void SetRotation(DirectX::XMFLOAT3 rot) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 rot;
};