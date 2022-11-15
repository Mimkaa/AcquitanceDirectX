#pragma once
#include "IndexedTriangleList.h"
#include <DirectXMath.h>
#include "ChiliMath.h"
#include <optional>
#include "Vertex.h"

class Sphere
{
public:

	static IndexedTriangleList MakeTesselated(Dvtx::VertexLayout layout_in, int latDiv, int longDiv)
	{
		namespace dx = DirectX;
		assert(latDiv >= 3);
		assert(longDiv >= 3);

		constexpr float radius = 1.0f;
		const auto base = dx::XMVectorSet(0.0f, 0.0f, radius, 0.0f);
		const float lattitudeAngle = PI / latDiv;
		const float longitudeAngle = 2.0f * PI / longDiv;

		Dvtx::VertexBuffer vertices{ std::move(layout_in) };

		for (int iLat = 1; iLat < latDiv; iLat++)
		{
			const auto latBase = dx::XMVector3Transform(
				base,
				dx::XMMatrixRotationX(lattitudeAngle * iLat)
			);
			for (int iLong = 0; iLong < longDiv; iLong++)
			{

				dx::XMFLOAT3 emplacer;
				auto v = dx::XMVector3Transform(
					latBase,
					dx::XMMatrixRotationZ(longitudeAngle * iLong)
				);
				dx::XMStoreFloat3(&emplacer, v);
				vertices.EmplaceBack(std::move(emplacer));
			}
		}

		// add the cap vertices
		const auto iNorthPole = (unsigned short)vertices.Size();
		dx::XMFLOAT3 emplacerN;
		dx::XMStoreFloat3(&emplacerN, base);
		vertices.EmplaceBack(std::move(emplacerN));

		const auto iSouthPole = (unsigned short)vertices.Size();
		dx::XMFLOAT3 emplacerS;
		dx::XMStoreFloat3(&emplacerS, dx::XMVectorNegate(base));
		vertices.EmplaceBack(std::move(emplacerS));

		const auto calcIdx = [latDiv, longDiv](unsigned short iLat, unsigned short iLong)
		{ return iLat * longDiv + iLong; };
		std::vector<unsigned short> indices;
		for (unsigned short iLat = 0; iLat < latDiv - 2; iLat++)
		{
			for (unsigned short iLong = 0; iLong < longDiv - 1; iLong++)
			{
				indices.push_back(calcIdx(iLat, iLong));
				indices.push_back(calcIdx(iLat + 1, iLong));
				indices.push_back(calcIdx(iLat, iLong + 1));
				indices.push_back(calcIdx(iLat, iLong + 1));
				indices.push_back(calcIdx(iLat + 1, iLong));
				indices.push_back(calcIdx(iLat + 1, iLong + 1));
			}
			// wrap band
			indices.push_back(calcIdx(iLat, longDiv - 1));
			indices.push_back(calcIdx(iLat + 1, longDiv - 1));
			indices.push_back(calcIdx(iLat, 0));
			indices.push_back(calcIdx(iLat, 0));
			indices.push_back(calcIdx(iLat + 1, longDiv - 1));
			indices.push_back(calcIdx(iLat + 1, 0));
		}

		// cap fans
		for (unsigned short iLong = 0; iLong < longDiv - 1; iLong++)
		{
			// north
			indices.push_back(iNorthPole);
			indices.push_back(calcIdx(0, iLong));
			indices.push_back(calcIdx(0, iLong + 1));
			// south
			indices.push_back(calcIdx(latDiv - 2, iLong + 1));
			indices.push_back(calcIdx(latDiv - 2, iLong));
			indices.push_back(iSouthPole);
		}
		// wrap triangles
		// north
		indices.push_back(iNorthPole);
		indices.push_back(calcIdx(0, longDiv - 1));
		indices.push_back(calcIdx(0, 0));
		// south
		indices.push_back(calcIdx(latDiv - 2, 0));
		indices.push_back(calcIdx(latDiv - 2, longDiv - 1));
		indices.push_back(iSouthPole);

		return { std::move(vertices),std::move(indices) };
	}

	static IndexedTriangleList Make(std::optional<Dvtx::VertexLayout> layout = std::nullopt )
	{
		if (!layout)
		{
			layout = Dvtx::VertexLayout{}.Append(Dvtx::VertexLayout::Position3D);
			
		}
		return MakeTesselated(std::move(*layout), 12, 24);
	}
};
