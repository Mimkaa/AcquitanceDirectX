#pragma once

#include "Surface.h"
#include "ChiliMath.h"
#include <string>
#include <DirectXMath.h>
#include <filesystem>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h> 
#include "Mesh.h"
#include "ModelException.h"
class NormalTwerker
{
public:
	template <class F>
	void static TransformFile(const std::string& pathIn, const std::string& pathOut, F&& func)
	{
		using namespace DirectX;
		auto sIn = Surface::FromFile(pathIn);

		const auto nPixels = sIn.GetHeight() * sIn.GetWidth();
		const auto pBegin = sIn.GetBufferPtr();
		const auto pEnd = pBegin + nPixels;
		for (auto pCurrent = pBegin; pCurrent < pEnd; pCurrent++)
		{
			auto n = ColorToVector(*pCurrent);
			*pCurrent = VectorToColor(func(n));
		}
		sIn.Save(pathOut);

	}
	void static TransformAllFiles(const std::string& objPath)
	{
		using namespace DirectX;
		const auto parPath = std::filesystem::path(objPath).parent_path().string() + "\\";
		Assimp::Importer importer;
		const auto scene = importer.ReadFile(objPath.c_str(),0u);

		const auto yFlip = XMVectorSet(1.0f, -1.0f, 1.0f, 1.0f);

		const auto func = [yFlip](XMVECTOR transVec) -> XMVECTOR
		{
			const auto result = XMVectorMultiply(transVec, yFlip);
			return result;
		};

		if (!scene)
		{
			throw ModelException(__LINE__, __FILE__, importer.GetErrorString());
		}

		for (int i = 0; i < scene->mNumMaterials; i++)
		{
			aiString textureSrc;
			auto& mat = *scene->mMaterials[i];
			if (mat.GetTexture(aiTextureType_NORMALS, 0, &textureSrc) == aiReturn_SUCCESS)
			{
				const auto inpPath = parPath + textureSrc.C_Str();
				const auto endPath = parPath + "myTextures\\" + textureSrc.C_Str();
				TransformFile(inpPath, endPath, func);
			}
		}


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