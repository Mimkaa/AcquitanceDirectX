#include "FirstMesh.h"
#include "BindableBase.h"
#include "GraphicsThrowHeader.h"
#include "imgui/imgui.h"
#include <format>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <vector>
FirstMesh::FirstMesh(Graphics& gfx, std::mt19937& rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist,

	DirectX::XMFLOAT3 mat, float scale)
	:
	TestObj(gfx, rng, adist, ddist, odist, rdist)

{
	if (!IsStaticInitialized())
	{
		namespace dx = DirectX;

		struct Vertex
		{
			dx::XMFLOAT3 pos;
			dx::XMFLOAT3 n;
		};
		Assimp::Importer importer;

		const auto scene = importer.ReadFile("Models\\suzanne.obj",
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices);

		// extract a mesh
		const auto mesh = scene->mMeshes[0];
		// reinterpret verices
		std::vector<Vertex> vertices;
		vertices.reserve(mesh->mNumVertices);
		for (int i = 0; i < mesh->mNumVertices; i++)
		{
			vertices.push_back({
				{mesh->mVertices[i].x * scale,mesh->mVertices[i].y * scale ,mesh->mVertices[i].z * scale},
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh->mNormals[i]) }
			);
		}
		// reinterpert indices
		std::vector<unsigned short> indices;
		indices.reserve(mesh->mNumFaces * 3);
		for (int i = 0; i < mesh->mNumFaces; i++)
		{
			indices.push_back(mesh->mFaces[i].mIndices[0]);
			indices.push_back(mesh->mFaces[i].mIndices[1]);
			indices.push_back(mesh->mFaces[i].mIndices[2]);
		}

		AddStaticBind(std::make_unique<VertexBuffer>(gfx, vertices));




		auto pvs = std::make_unique<VertexShader>(gfx, L"PhongVS.cso");
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<PixelShader>(gfx, L"PhongPS.cso"));

		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, indices));


		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{"Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
			{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};
		AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));
		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else
	{
		SetIndexFromStatic();
	}
	AddBind(std::make_unique<TransformCbuf>(gfx, *this, 1u));


	materialConstants.color = mat;
	AddBind(std::make_unique<MaterialCBuf>(gfx, materialConstants, 1u));

	


}



DirectX::XMMATRIX FirstMesh::GetTransformXM() const noexcept
{
	namespace dx = DirectX;
	return TestObj::GetTransformXM() ;
}

