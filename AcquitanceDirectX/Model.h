#pragma once
#include "DrawableBase.h"
#include "BindableBase.h"
#include <vector>
#include <memory>
#include <DirectXMath.h>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include "Vertex.h"
class Mesh:public DrawableBase<Mesh>
{
public:
	Mesh(Graphics& gfx, std::vector<std::unique_ptr<Bindable>>binds_in)
	{
		if (!IsStaticInitialized())
		{
			AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
		}
		for (auto& b : binds_in)
		{
			if (auto ptr = dynamic_cast<IndexBuffer*>(b.get()))
			{
				AddIndexBuffer(std::unique_ptr<IndexBuffer>(ptr));
				b.release();
			}
			else
			{
				AddBind(std::move(b));
			}
		}
		AddBind(std::make_unique<TransformCbuf>(gfx, *this, 1u));
	}

	DirectX::XMMATRIX GetTransformXM() const noexcept
	{
		return XMLoadFloat4x4(&transform);
	}

	void Draw(Graphics& gfx, const DirectX::FXMMATRIX& accumulatedTransform) const noexcept(!IS_DEBUG)
	{
		DirectX::XMStoreFloat4x4(&transform, accumulatedTransform);
		Drawable::Draw(gfx);
	}

	
private:
	mutable DirectX::XMFLOAT4X4 transform;
};


class Node
{
	friend class Model;
public:
	Node(std::vector<Mesh*> meshes_in, const DirectX::XMMATRIX& transfomation)
		:
		meshes(std::move(meshes_in))

	{
		DirectX::XMStoreFloat4x4(&transform, transfomation);
	}
	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform)noexcept(!IS_DEBUG)
	{
		const auto build = XMLoadFloat4x4(&transform) * accumulatedTransform;
		for (auto& m : meshes)
		{
			m->Draw(gfx, build);
		}
		for (auto& c : children)
		{
			c->Draw(gfx, build);
		}
	}
	
private:
	void AddChild(std::unique_ptr<Node> node_in)
	{
		assert(node_in);
		children.push_back(std::move(node_in));
	}
private:
	std::vector<Mesh*> meshes;
	std::vector<std::unique_ptr<Node>> children;
	DirectX::XMFLOAT4X4 transform;
};


class Model
{
	
public:
	Model(Graphics& gfx, const char* filename)
		:
		gfx(gfx)
	{
		Assimp::Importer importer;
		const auto scene = importer.ReadFile(filename,
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices);
		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			ParseMesh(scene->mMeshes[i],1.0f);
		}
		pRoot=ParseNode(scene->mRootNode);
	}
private:
	void ParseMesh(const aiMesh* mesh_in, float scale)
	{
		namespace dx = DirectX;
		std::vector<std::unique_ptr<Bindable>> currBinds;

		using hw3dexp::VertexLayout;
		hw3dexp::VertexBuffer vb{ std::move(VertexLayout{}
		.Append(VertexLayout::Position3D)
		.Append(VertexLayout::Normal)) };

		for (int i = 0; i < mesh_in->mNumVertices; i++)
		{
			vb.EmplaceBack(
				dx::XMFLOAT3{ mesh_in->mVertices[i].x * scale,mesh_in->mVertices[i].y * scale ,mesh_in->mVertices[i].z * scale },
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh_in->mNormals[i])
			);
		}

		// reinterpert indices
		std::vector<unsigned short> indices;
		indices.reserve(mesh_in->mNumFaces * 3);
		for (int i = 0; i < mesh_in->mNumFaces; i++)
		{
			indices.push_back(mesh_in->mFaces[i].mIndices[0]);
			indices.push_back(mesh_in->mFaces[i].mIndices[1]);
			indices.push_back(mesh_in->mFaces[i].mIndices[2]);
		}
		currBinds.push_back(std::make_unique<VertexBuffer>( gfx, vb));

		currBinds.push_back(std::make_unique<IndexBuffer>(gfx, indices));

		auto pvs = std::make_unique<VertexShader>(gfx, L"PhongVS.cso");
		auto pvsbc = pvs->GetBytecode();
		currBinds.push_back(std::move(pvs));

		currBinds.push_back(std::make_unique<PixelShader>(gfx, L"PhongPS.cso"));

		


		currBinds.push_back(std::make_unique<InputLayout>(gfx, vb.GetLayout().GetD3DLayout(), pvsbc));

		struct PSMaterialConstants
		{
			DirectX::XMFLOAT3 color = { 0.6f,0.6f,0.8f };
			float specularIntensity = 0.6f;
			float specularPower = 30.0f;
			float padding[3];

		} materialConstants;

		currBinds.push_back(std::make_unique<PixelConstantBuffer<PSMaterialConstants>>(gfx, materialConstants, 1u));

		meshes.push_back(std::make_unique<Mesh>(gfx, std::move(currBinds)));
		
	}

	std::unique_ptr<Node> ParseNode( aiNode* node_in)
	{
		const auto transform = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(&node_in->mTransformation)));

		std::vector<Mesh*> nodeMeshes;
		nodeMeshes.reserve(node_in->mNumMeshes);
		for (int i = 0; i < node_in->mNumMeshes; i++)
		{
			const auto mesh_index = node_in->mMeshes[i];
			nodeMeshes.push_back(meshes.at(mesh_index).get());
		}
		auto pNode = std::make_unique<Node>(std::move(nodeMeshes), transform);
		for (int i = 0; i < node_in->mNumChildren; i++)
		{
			pNode->AddChild(ParseNode(node_in->mChildren[i]));
		}
		return pNode;
	}
public :
	void Draw()
	{
		pRoot->Draw(gfx, DirectX::XMMatrixIdentity());
	}

private:
	std::vector<std::unique_ptr<Mesh>> meshes;
	std::unique_ptr<Node> pRoot;
	Graphics& gfx;
};

