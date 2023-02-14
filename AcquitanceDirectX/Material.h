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
	std::vector<unsigned short> ExtractIndices(const aiMesh& mesh_in) const noexcept;

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