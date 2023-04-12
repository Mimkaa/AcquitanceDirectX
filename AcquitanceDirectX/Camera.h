#pragma once
#include "Graphics.hpp"
#include <string>
#include "Projection.h"
#include "CameraIndicator.h"
#include "Frustrum.h"

class FrameComander;

class Graphics;

class Camera
{
public :
	Camera(Graphics& gfx, const std::string& name_in, float pitch, float yaw, DirectX::XMFLOAT3 v);
	DirectX::XMMATRIX GetMatrix() const noexcept;
	void SpawnWidgets() noexcept;
	void Reset() noexcept;
	void BindToGraphics(Graphics& gfx);
	void Rotate(const int dx, const int dy);
	void Translate(DirectX::XMFLOAT3 translation);
	DirectX::XMVECTOR GetPosition();
	std::string GetName() const;
	void Submit(FrameComander& fc);

private:
	std::string name;
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 OrPos;
	float pitch = 1.2f;
	float yaw = 0.0f;
	Projection prj;
	CameraIndicator camInd;
	Frustrum frus;
	static constexpr float rotationSpeed = 0.004f;
	static constexpr float movingSpeed = 12.0f;

	
};