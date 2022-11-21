#pragma once
#include <vector>
#include <DirectXMath.h>
#include "Vertex.h"
class IndexedTriangleList
{
public:
	IndexedTriangleList() = default;
	IndexedTriangleList(Dvtx::VertexBuffer verts_in, std::vector<unsigned short> indices_in)
		:
		vertices(std::move(verts_in)),
		indices(std::move(indices_in))
	{
		assert(vertices.Size() > 2);
		assert(indices.size() % 3 == 0);
	}

	void SetNormalsIndependentFlat() noxnd
	{
		using Type = Dvtx::VertexLayout::ElementType;
		using namespace DirectX;
		assert(indices.size() % 3 == 0 && indices.size() > 0);
		for (int j = 0; j < indices.size() / 3; j++)
		{
			const int i0 = indices[j * 3 + 0];
			const int i1 = indices[j * 3 + 1];
			const int i2 = indices[j * 3 + 2];
			auto v0 = vertices[i0];
			auto v1 = vertices[i1];
			auto v2 = vertices[i2];
			const auto p0 = XMLoadFloat3(&v0.Attr<Type::Position3D>());
			const auto p1 = XMLoadFloat3(&v1.Attr<Type::Position3D>());
			const auto p2 = XMLoadFloat3(&v2.Attr<Type::Position3D>());
			const auto normal =XMVector3Normalize (XMVector3Cross((p1 - p0), (p2 - p0)));
			XMFLOAT3 norm;
			XMStoreFloat3(&norm, normal);
			vertices[i0].Attr<Type::Normal>() = norm;
			vertices[i1].Attr<Type::Normal>() = norm;
			vertices[i2].Attr<Type::Normal>() = norm;
			
		}
		
	}


	void Transform(DirectX::FXMMATRIX matrix)
	{
		for (int i = 0; i < vertices.Size(); i++)
		{
			auto& attr = vertices[i].Attr<Dvtx::VertexLayout::Position3D>();
			const DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&attr);
			DirectX::XMStoreFloat3(
				&attr,
				DirectX::XMVector3Transform(pos, matrix)
			);
		}
		
	}

public:
	Dvtx::VertexBuffer vertices;
	std::vector<unsigned short> indices;
};
