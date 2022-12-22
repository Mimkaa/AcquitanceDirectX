#pragma once

#include "Surface.h"
#include "ChiliMath.h"
#include <string>
#include <DirectXMath.h>

class NormalTwerker
{
public:
	void static RotateXAxis180(const std::string& pathIn, const std::string& pathOut)
	{
		using namespace DirectX;
		const auto rotation = XMMatrixRotationX(PI);
		auto sIn = Surface::FromFile(pathIn);

		const auto nPixels = sIn.GetHeight() * sIn.GetWidth();
		const auto pBegin = sIn.GetBufferPtr();
		const auto pEnd = pBegin + nPixels;
		for (auto pCurrent = pBegin; pCurrent < pEnd; pCurrent++)
		{
			auto n = ColorToVector(*pCurrent);
			const auto transformedV = XMVector3Transform(n, rotation);
			*pCurrent = VectorToColor(transformedV);
		}
		sIn.Save(pathOut);

	}
	static void RotateXAxis180(const std::string& pathIn)
	{
		return RotateXAxis180(pathIn, pathIn);
	}
private:
	static DirectX::XMVECTOR ColorToVector(Surface::Color c)
	{
		using namespace DirectX;
		auto colorVec = XMVectorSet(c.GetR(), c.GetG(), c.GetB(), 0.0f);
		const auto convFrom255 = XMVectorReplicate(2.0f / 255.0f);
		colorVec = XMVectorMultiply(colorVec, convFrom255);
		const auto all1 = XMVectorReplicate(1.0f);
		colorVec = XMVectorSubtract(colorVec, all1);
		return colorVec;
	}

	static Surface::Color VectorToColor(DirectX::XMVECTOR n)
	{
		using namespace DirectX;
		const auto all1 = XMVectorReplicate(1.0f);
		XMVECTOR nOut = XMVectorAdd(n, all1);
		const auto all255 = XMVectorReplicate(255.0f / 2.0f);
		nOut = XMVectorMultiply(nOut, all255);
		XMFLOAT3 floats;
		XMStoreFloat3(&floats, nOut);
		return {
			(unsigned char)round(floats.x),
			(unsigned char)round(floats.y),
			(unsigned char)round(floats.z),
		};
	}

};