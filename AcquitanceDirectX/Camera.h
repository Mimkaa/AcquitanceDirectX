#pragma once
#include "Graphics.hpp"

class Camera
{
public :
	Camera();
	DirectX::XMMATRIX GetMatrix() const noexcept;
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;
	void Rotate(const int dx, const int dy);
	void Translate(DirectX::XMFLOAT3 translation);
	DirectX::XMVECTOR GetPosition();

private:
	DirectX::XMFLOAT3 pos;
	float pitch = 1.2f;
	float yaw = 0.0f;
	static constexpr float rotationSpeed = 0.004f;
	static constexpr float movingSpeed = 12.0f;
	
};