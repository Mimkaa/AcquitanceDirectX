#pragma once
#include <assimp/scene.h>
#include "BindableCommon.h"
#include <vector>
#include <filesystem>
#include "Technique.h"
#include "Vertex.h"
#include <assimp/scene.h>           // Output data structure
#include <filesystem>
#include "BindableCommon.h"
class Material
{
public:
	Material(Graphics& gfx, const aiMaterial& material, std::filesystem::path path);
	std::vector<unsigned short> ExtractIndices(const aiMesh& mesh_in) const noexcept
	{
		std::vector<unsigned short> indices;
		indices.reserve(mesh_in.mNumFaces * 3);
		for (int i = 0; i < mesh_in.mNumFaces; i++)
		{
			const auto& face = mesh_in.mFaces[i];
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}
		return indices;
	}
	Dvtx::VertexBuffer ExtractVertices(const aiMesh& mesh_in) const noexcept;

	std::shared_ptr<Bind::IndexBuffer> CtreateIndexBuffer(Graphics& gfx, const aiMesh& mesh_in) const noexcept
	{
		return Bind::IndexBuffer::Resolve(gfx, GenerateMeshTag(mesh_in), ExtractIndices(mesh_in));
	}

	std::shared_ptr<Bind::VertexBuffer> CtreateVertexBuffer(Graphics& gfx, const aiMesh& mesh_in) const noexcept
	{
		return Bind::VertexBuffer::Resolve(gfx, GenerateMeshTag(mesh_in), ExtractVertices(mesh_in));
	}

	std::string GenerateMeshTag(const aiMesh& mesh_in) const
	{
		return ModelPath + "%" + mesh_in.mName.C_Str();
	}

	std::vector<Technique> GetTechniques() const noexcept;
private:
	Dvtx::VertexLayout vtxLayout;
	std::vector<Technique> techniques;
	std::string ModelPath;
	std::string name;
};