#pragma once
#include <DirectXMath.h>

class Projection
{
public:
	Projection(float width, float height, float near, float far);
	DirectX::XMMATRIX GetMatrix() const;
	void SpawnWidges();
private:
	float width;
	float height;
	float nearr;
	float farr;


};