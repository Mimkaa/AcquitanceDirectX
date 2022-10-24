#pragma once
#include "IndexedTriangleList.h"
#include <DirectXMath.h>

class Cube
{
public:
	template<class V>
	static IndexedTriangleList<V> Make()
	{
		namespace dx = DirectX;

		constexpr float side = 1.0f / 2.0f;

		std::vector<dx::XMFLOAT3> vertices;
		vertices.emplace_back(-side, -side, -side); // 0
		vertices.emplace_back(side, -side, -side); // 1
		vertices.emplace_back(-side, side, -side); // 2
		vertices.emplace_back(side, side, -side); // 3
		vertices.emplace_back(-side, -side, side); // 4
		vertices.emplace_back(side, -side, side); // 5
		vertices.emplace_back(-side, side, side); // 6
		vertices.emplace_back(side, side, side); // 7

		std::vector<V> verts(vertices.size());
		for (size_t i = 0; i < vertices.size(); i++)
		{
			verts[i].pos = vertices[i];
		}
		return{
			std::move(verts),{
				0,2,1, 2,3,1,
				1,3,5, 3,7,5,
				2,6,3, 3,6,7,
				4,5,7, 4,7,6,
				0,4,2, 2,4,6,
				0,1,4, 1,5,4
			}
		};
	}

	template<class V>
	static IndexedTriangleList<V> MakeIndependent(float size = 1.0f)
	{
		namespace dx = DirectX;

		const float side = size / 2.0f;

		std::vector<dx::XMFLOAT3> vertices;

		vertices.emplace_back(-side, -side, -side); // 0 near side
		vertices.emplace_back(side, -side, -side); // 1
		vertices.emplace_back(-side, side, -side); // 2
		vertices.emplace_back(side, side, -side); // 3
		vertices.emplace_back(-side, -side, side); // 4 far side
		vertices.emplace_back(side, -side, side); // 5
		vertices.emplace_back(-side, side, side); // 6
		vertices.emplace_back(side, side, side); // 7
		vertices.emplace_back(-side, -side, -side); // 8 left side
		vertices.emplace_back(-side, side, -side); // 9
		vertices.emplace_back(-side, -side, side); // 10
		vertices.emplace_back(-side, side, side); // 11
		vertices.emplace_back(side, -side, -side); // 12 right side
		vertices.emplace_back(side, side, -side); // 13
		vertices.emplace_back(side, -side, side); // 14
		vertices.emplace_back(side, side, side); // 15
		vertices.emplace_back(-side, -side, -side); // 16 bottom side
		vertices.emplace_back(side, -side, -side); // 17
		vertices.emplace_back(-side, -side, side); // 18
		vertices.emplace_back(side, -side, side); // 19
		vertices.emplace_back(-side, side, -side); // 20 top side
		vertices.emplace_back(side, side, -side); // 21
		vertices.emplace_back(-side, side, side); // 22
		vertices.emplace_back(side, side, side); // 23

		std::vector<V> verts(vertices.size());
		for (size_t i = 0; i < vertices.size(); i++)
		{
			verts[i].pos = vertices[i];
		}
		return{
			std::move(verts),{
				0,2, 1,    2,3,1,
				4,5, 7,    4,7,6,
				8,10, 9,  10,11,9,
				12,13,15, 12,15,14,
				16,17,18, 18,17,19,
				20,23,21, 20,22,23
			}
		};
	}

	template<class V>
	static IndexedTriangleList<V> MakeSkinnedIndependent(float size = 1.0f)
	{
		namespace dx = DirectX;
		const float side = size / 2.0f;

		std::vector<dx::XMFLOAT3> vertices;
		// positions
		vertices.emplace_back(-side, -side, -side); // 0 near side
		vertices.emplace_back(side, -side, -side); // 1
		vertices.emplace_back(-side, side, -side); // 2
		vertices.emplace_back(side, side, -side); // 3
		vertices.emplace_back(-side, -side, side); // 4 far side
		vertices.emplace_back(side, -side, side); // 5
		vertices.emplace_back(-side, side, side); // 6
		vertices.emplace_back(side, side, side); // 7
		vertices.emplace_back(-side, -side, -side); // 8 left side
		vertices.emplace_back(-side, side, -side); // 9
		vertices.emplace_back(-side, -side, side); // 10
		vertices.emplace_back(-side, side, side); // 11
		vertices.emplace_back(side, -side, -side); // 12 right side
		vertices.emplace_back(side, side, -side); // 13
		vertices.emplace_back(side, -side, side); // 14
		vertices.emplace_back(side, side, side); // 15
		vertices.emplace_back(-side, -side, -side); // 16 bottom side
		vertices.emplace_back(side, -side, -side); // 17
		vertices.emplace_back(-side, -side, side); // 18
		vertices.emplace_back(side, -side, side); // 19
		vertices.emplace_back(-side, side, -side); // 20 top side
		vertices.emplace_back(side, side, -side); // 21
		vertices.emplace_back(-side, side, side); // 22
		vertices.emplace_back(side, side, side); // 23

		std::vector<V> verts(vertices.size());
		int count = 0;
		float tex_coors[4][2] = { {0.0f, 0.0f},{1.0f, 0.0f},{0.0f, 1.0f},{1.0f, 1.0f} };
		for (size_t i = 0; i < vertices.size(); i++)
		{
			if (count > 3)
			{
				count = 0;
			}
			verts[i].pos = vertices[i];
			verts[i].tex = { tex_coors[count][0],tex_coors[count][1] };
			count += 1;
			
		}
		return{
			std::move(verts),{
				0,2, 1,    2,3,1,
				4,5, 7,    4,7,6,
				8,10, 9,  10,11,9,
				12,13,15, 12,15,14,
				16,17,18, 18,17,19,
				20,23,21, 20,22,23
			}
		};

	}

	template<class V>
	static IndexedTriangleList<V> MakeSkinned()
	{
		namespace dx = DirectX;

		constexpr float side = 1.0f / 2.0f;

		std::vector<V> vertices(14);

		vertices[0].pos = { -side,-side,-side };
		vertices[0].tex = { 2.0f / 3.0f,0.0f / 4.0f };
		vertices[1].pos = { side,-side,-side };
		vertices[1].tex = { 1.0f / 3.0f,0.0f / 4.0f };
		vertices[2].pos = { -side,side,-side };
		vertices[2].tex = { 2.0f / 3.0f,1.0f / 4.0f };
		vertices[3].pos = { side,side,-side };
		vertices[3].tex = { 1.0f / 3.0f,1.0f / 4.0f };
		vertices[4].pos = { -side,-side,side };
		vertices[4].tex = { 2.0f / 3.0f,3.0f / 4.0f };
		vertices[5].pos = { side,-side,side };
		vertices[5].tex = { 1.0f / 3.0f,3.0f / 4.0f };
		vertices[6].pos = { -side,side,side };
		vertices[6].tex = { 2.0f / 3.0f,2.0f / 4.0f };
		vertices[7].pos = { side,side,side };
		vertices[7].tex = { 1.0f / 3.0f,2.0f / 4.0f };
		vertices[8].pos = { -side,-side,-side };
		vertices[8].tex = { 2.0f / 3.0f,4.0f / 4.0f };
		vertices[9].pos = { side,-side,-side };
		vertices[9].tex = { 1.0f / 3.0f,4.0f / 4.0f };
		vertices[10].pos = { -side,-side,-side };
		vertices[10].tex = { 3.0f / 3.0f,1.0f / 4.0f };
		vertices[11].pos = { -side,-side,side };
		vertices[11].tex = { 3.0f / 3.0f,2.0f / 4.0f };
		vertices[12].pos = { side,-side,-side };
		vertices[12].tex = { 0.0f / 3.0f,1.0f / 4.0f };
		vertices[13].pos = { side,-side,side };
		vertices[13].tex = { 0.0f / 3.0f,2.0f / 4.0f };

		return{
			std::move(vertices),{
				0,2,1,   2,3,1,
				4,8,5,   5,8,9,
				2,6,3,   3,6,7,
				4,5,7,   4,7,6,
				2,10,11, 2,11,6,
				12,3,7,  12,7,13
			}
		};
	}
};
