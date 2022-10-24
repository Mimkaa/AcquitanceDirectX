#pragma once
#include "IndexedTriangleList.h"
#include <DirectXMath.h>
#include "ChiliMath.h"
#include <vector>

class MyCone {
public:
	// struct and class are almost the same after all
	template<class V>
	static IndexedTriangleList<V> MakeTessalated(int divs = 3, float side = 1.0f)
	{
		namespace dx = DirectX;
		assert("the base has to consist at lest of 3 vertices" && divs >= 3);
		const auto rotor = dx::XMVectorSet(0.0f, -side/2, 0.0f, 0.0f);
		// create vertices
		std::vector<V> vertices;
		const float dAngle = (PI * 2) / (divs);


		// add base
		vertices.emplace_back();
		dx::XMStoreFloat3(&vertices[vertices.size() - 1].pos, dx::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f));
		// creating bottom vertices
		for (int i = 0; i < divs; i++)
		{
			vertices.emplace_back();
			dx::XMStoreFloat3(&vertices[vertices.size() - 1].pos, dx::XMVector3Transform(rotor, dx::XMMatrixRotationZ(i * dAngle)));
		}

		

		// create cone vertices
		std::vector<V> cone_vertices;
		for (int i = 0; i < divs; i++)
		{
			float thethas[] = {dAngle * i,  dAngle * ((i+1) % (divs)) };
			cone_vertices.emplace_back();
			dx::XMStoreFloat3(&cone_vertices.back().pos, dx::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f));
			for (int j = 0; j < sizeof(thethas) / sizeof(thethas[0]); j++)
			{
				cone_vertices.emplace_back();
				dx::XMStoreFloat3(&cone_vertices.back().pos, dx::XMVector3Transform(rotor, dx::XMMatrixRotationZ(thethas[j])));
			}

		
		}
		
		

		// create indices
	
		std::vector<unsigned short> indices;
		
	
		
		// cone 
		int overal_size = 0;
		for (int i = 0; i < cone_vertices.size(); i++)
		{
			indices.push_back(i);
			overal_size += 1;
		}
		
		// base
		for (int i = overal_size+1; i < overal_size + divs + 1; i++)
		{
			indices.push_back(overal_size );
			indices.push_back(((i + 1) % (overal_size + divs+1 )) > 0 ? (i + 1) % (overal_size + divs+1) : overal_size+1);
			indices.push_back(i);
			

		}

		cone_vertices.insert(cone_vertices.end(), vertices.begin(), vertices.end());
		return { std::move(cone_vertices), std::move(indices) };
	}

};
	
