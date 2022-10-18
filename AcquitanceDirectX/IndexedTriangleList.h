#pragma once
#include <vector>
#include <DirectXMath.h>

template<class T>
class IndexedTriangleList
{
public:
	IndexedTriangleList() = default;
	IndexedTriangleList(std::vector<T> verts_in, std::vector<unsigned short> indices_in)
		:
		vertices(std::move(verts_in)),
		indices(std::move(indices_in))
	{
		assert(vertices.size() > 2);
		assert(indices.size() % 3 == 0);
	}

	void SetNormalsIndependentFlat() noexcept(!IS_DEBUG)
	{
		using namespace DirectX;
		assert(indices.size() % 3 == 0 && indices.size() > 0);
		for (int j = 0; j < indices.size() / 3; j++)
		{
			const int i0 = indices[j * 3 + 0];
			const int i1 = indices[j * 3 + 1];
			const int i2 = indices[j * 3 + 2];
			auto& v0 = vertices[i0];
			auto& v1 = vertices[i1];
			auto& v2 = vertices[i2];
			const auto p0 = XMLoadFloat3(&v0.pos);
			const auto p1 = XMLoadFloat3(&v1.pos);
			const auto p2 = XMLoadFloat3(&v2.pos);
			const auto normal =XMVector3Normalize (XMVector3Cross((p1 - p0), (p2 - p0)));
			XMStoreFloat3(&v0.n, normal);
			XMStoreFloat3(&v1.n, normal);
			XMStoreFloat3(&v2.n, normal);
		}
		
	}


	void Transform(DirectX::FXMMATRIX matrix)
	{
		for (auto& v : vertices)
		{
			const DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&v.pos);
			DirectX::XMStoreFloat3(
				&v.pos,
				DirectX::XMVector3Transform(pos, matrix)
			);
		}
	}

public:
	std::vector<T> vertices;
	std::vector<unsigned short> indices;
};
