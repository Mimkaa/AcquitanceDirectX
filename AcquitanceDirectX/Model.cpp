#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "ModelException.h"
#include "ModelWindow.h"
#include "Node.h"
#include "Mesh.h"

namespace dx = DirectX;

Model::Model(Graphics& gfx, const std::string& pathname, const float scale)
	:
	gfx(gfx),
	pWindow(std::make_unique<ModelWindow>())
{
	Assimp::Importer importer;
	const auto scene = importer.ReadFile(pathname.c_str(),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace
	);
	if (!scene)
	{
		throw ModelException(__LINE__, __FILE__, importer.GetErrorString());
	}
	for (int i = 0; i < scene->mNumMeshes; i++)
	{
		ParseMesh(scene->mMeshes[i], scale, scene->mMaterials, pathname);
	}
	int firstNodeId = 0;
	pRoot = ParseNode(firstNodeId, scene->mRootNode);

}

void  Model::SetRootTransform(const DirectX::FXMMATRIX& rt) noexcept
{
	pRoot->SetAppliedTransform(rt);
}

Model::~Model() noexcept
{
}


void Model::ParseMesh(const aiMesh* mesh_in, float scale, const aiMaterial* const* ppMaterials, const std::filesystem::path& path)
{
	
}

void Model::Submit(FrameComander& frame) const noxnd
{
	pRoot->Submit(frame, dx::XMMatrixIdentity());
}

void Model::ShowWindow(Graphics& gfx, const char* windowName) noexcept
{
	pWindow->Show(gfx, windowName, *pRoot);
}



std::unique_ptr<Node> Model::ParseNode(int& node_id, aiNode* node_in)
{
	const auto transform = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(&node_in->mTransformation)));

	std::vector<Mesh*> nodeMeshes;
	nodeMeshes.reserve(node_in->mNumMeshes);
	for (int i = 0; i < node_in->mNumMeshes; i++)
	{
		const auto mesh_index = node_in->mMeshes[i];
		nodeMeshes.push_back(meshes.at(mesh_index).get());
	}
	auto pNode = std::make_unique<Node>(node_id++, node_in->mName.C_Str(), std::move(nodeMeshes), transform);
	for (int i = 0; i < node_in->mNumChildren; i++)
	{
		pNode->AddChild(ParseNode(node_id, node_in->mChildren[i]));
	}
	return pNode;
}