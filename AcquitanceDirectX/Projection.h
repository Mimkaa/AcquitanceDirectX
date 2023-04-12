#pragma once
#include <DirectXMath.h>

class Projection
{
public:
	Projection(float width, float height, float near, float far);
	DirectX::XMMATRIX GetMatrix() const;
	void SpawnWidges();
	float GetWidth() const
	{
		return width;
	}
	float GetHeight() const
	{
		return height;
	}
	float GetNear() const
	{
		return nearr;
	}
	float GetFar() const
	{
		return farr;
	}
private:
	float width;
	float height;
	float nearr;
	float farr;


};