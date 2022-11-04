#pragma once
#include "DrawableBase.h"
#include "BindableCommon.h"
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
	Mesh(Graphics& gfx, std::vector<std::unique_ptr<Bind::Bindable>>binds_in);

	DirectX::XMMATRIX GetTransformXM() const noexcept;

	void Draw(Graphics& gfx, const DirectX::FXMMATRIX& accumulatedTransform) const noxnd;
private:
	mutable DirectX::XMFLOAT4X4 transform;
};


class Node
{
	friend class Model;
public:
	Node(std::vector<Mesh*> meshes_in, const DirectX::XMMATRIX& transfomation);
	
	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform)noxnd;
	
	
private:
	void AddChild(std::unique_ptr<Node> node_in);
	
private:
	std::vector<Mesh*> meshes;
	std::vector<std::unique_ptr<Node>> children;
	DirectX::XMFLOAT4X4 transform;
};


class Model
{
	
public:
	Model(Graphics& gfx, const char* filename);
		
private:
	void ParseMesh(const aiMesh* mesh_in, float scale);
	
	std::unique_ptr<Node> ParseNode(aiNode* node_in);
	
	
public :
	void Draw(DirectX::FXMMATRIX transform);
	

private:
	std::vector<std::unique_ptr<Mesh>> meshes;
	std::unique_ptr<Node> pRoot;
	Graphics& gfx;
};

