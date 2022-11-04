#include "FirstMesh.h"
#include "BindableCommon.h"
#include "GraphicsThrowHeader.h"
#include "imgui/imgui.h"
#include <format>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <vector>
#include "Vertex.h"
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

		using Dvtx::VertexLayout;

		Dvtx::VertexBuffer vb{ std::move(VertexLayout{}.Append(VertexLayout::Position3D).Append(VertexLayout::Normal))};

		Assimp::Importer importer;

		const auto scene = importer.ReadFile("Models\\suzanne.obj",
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices);

		// extract a mesh
		const auto mesh = scene->mMeshes[0];
		
		for (int i = 0; i < mesh->mNumVertices; i++)
		{
			vb.EmplaceBack(
				dx::XMFLOAT3{mesh->mVertices[i].x * scale,mesh->mVertices[i].y * scale ,mesh->mVertices[i].z * scale},
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh->mNormals[i]) 
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

		AddStaticBind(std::make_unique<Bind::VertexBuffer>(gfx, vb));




		auto pvs = std::make_unique<Bind::VertexShader>(gfx, L"PhongVS.cso");
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<Bind::PixelShader>(gfx, L"PhongPS.cso"));

		AddStaticIndexBuffer(std::make_unique<Bind::IndexBuffer>(gfx, indices));


		AddStaticBind(std::make_unique<Bind::InputLayout>(gfx, vb.GetLayout().GetD3DLayout(), pvsbc));
		AddStaticBind(std::make_unique<Bind::Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else
	{
		SetIndexFromStatic();
	}
	AddBind(std::make_unique<Bind::TransformCbuf>(gfx, *this, 1u));


	materialConstants.color = mat;
	AddBind(std::make_unique<MaterialCBuf>(gfx, materialConstants, 1u));

	


}



DirectX::XMMATRIX FirstMesh::GetTransformXM() const noexcept
{
	namespace dx = DirectX;
	return TestObj::GetTransformXM() ;
}

