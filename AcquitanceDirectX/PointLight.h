#pragma once
#include "Graphics.hpp"
#include "ConstantBuffers.h"
#include "SolidSphere.h"
#include "Camera.h"
#include <memory>

class PointLight
{
public:
	PointLight(Graphics& gfx, float radius = 0.5f);
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;
	void Submit(class FrameComander& frame, size_t channel) const noxnd;
	void Bind(Graphics& gfx, DirectX::XMMATRIX view) const noexcept;
	std::shared_ptr<Camera> GetCamera();
private:
	struct PointLightCBuf
	{
		// alignas add padding (additional 4 bytes)
		alignas(16)DirectX::XMFLOAT3 pos;
		alignas(16)DirectX::XMFLOAT3 ambient;
		alignas(16)DirectX::XMFLOAT3 diffuseColor;
		float diffuseIntensity;
		float attConst;
		float attLin;
		float attQuad;
	};
private:

	PointLightCBuf cbData;
	std::shared_ptr<Camera> pCamera;
	mutable SolidSphere mesh;
	mutable Bind::PixelConstantBuffer<PointLightCBuf> cbuf;
}; 