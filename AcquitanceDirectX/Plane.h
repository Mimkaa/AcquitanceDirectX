#pragma once

#include <vector>
#include <array>
#include "Vertex.h"
#include "IndexedTriangleList.h"
#include "ChiliMath.h"

#include <optional>
class Plane
{
public:
	
	static IndexedTriangleList MakeTesselated(Dvtx::VertexLayout layout_in, int divisions_x, int divisions_y)
	{
		namespace dx = DirectX;
		assert(divisions_x >= 1);
		assert(divisions_y >= 1);

		Dvtx::VertexBuffer vertices{ std::move(layout_in) };

		constexpr float width = 2.0f;
		constexpr float height = 2.0f;
		const int nVertices_x = divisions_x + 1;
		const int nVertices_y = divisions_y + 1;
		

		{
			const float side_x = width / 2.0f;
			const float side_y = height / 2.0f;
			const float divisionSize_x = width / float(divisions_x);
			const float divisionSize_y = height / float(divisions_y);
			const auto bottomLeft = dx::XMVectorSet(-side_x, -side_y, 0.0f, 0.0f);

			for (int y = 0, i = 0; y < nVertices_y; y++)
			{
				const float y_pos = float(y) * divisionSize_y;
				for (int x = 0; x < nVertices_x; x++, i++)
				{
					dx::XMFLOAT3 emplacer;
					const auto v = dx::XMVectorAdd(
						bottomLeft,
						dx::XMVectorSet(float(x) * divisionSize_x, y_pos, 0.0f, 0.0f)
					);
					dx::XMStoreFloat3(&emplacer,v);
					vertices.EmplaceBack(std::move(emplacer), dx::XMFLOAT3(0.0f,0.0f,-1.0f), dx::XMFLOAT2(x, -y));
				}
			}
		}

		std::vector<unsigned short> indices;
		indices.reserve(sq(divisions_x * divisions_y) * 6);
		{
			const auto vxy2i = [nVertices_x](size_t x, size_t y)
			{
				return (unsigned short)(y * nVertices_x + x);
			};
			for (size_t y = 0; y < divisions_y; y++)
			{
				for (size_t x = 0; x < divisions_x; x++)
				{
					const std::array<unsigned short, 4> indexArray =
					{ vxy2i(x,y),vxy2i(x + 1,y),vxy2i(x,y + 1),vxy2i(x + 1,y + 1) };
					indices.push_back(indexArray[0]);
					indices.push_back(indexArray[2]);
					indices.push_back(indexArray[1]);
					indices.push_back(indexArray[1]);
					indices.push_back(indexArray[2]);
					indices.push_back(indexArray[3]);
				}
			}
		}

		return{ std::move(vertices),std::move(indices) };
	}
	
	static IndexedTriangleList Make(std::optional<Dvtx::VertexLayout> layout = std::nullopt)
	{
		if (!layout)
		{
			layout = Dvtx::VertexLayout{}.Append(Dvtx::VertexLayout::Position3D).
				Append(Dvtx::VertexLayout::Normal).Append(Dvtx::VertexLayout::Texture2D);

		}
		return MakeTesselated(std::move(*layout),1, 1);
	}
};
