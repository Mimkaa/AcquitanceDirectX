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
#include "imgui/imgui.h"
#include <optional>
#include "ChiliException.hpp"


class ModelException :public ChiliException
{
public:
	ModelException(int line, const char* file, std::string note) noexcept;
	const char* what() const noexcept override;
	const std::string& GetNote()const noexcept;
	const char* GetType() const noexcept override;
private:
	std::string note;
};

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
	friend class WindowMenu;
public:
	Node(int id_in, const std::string& name_in, std::vector<Mesh*> meshes_in, const DirectX::XMMATRIX& transfomation);
	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) noxnd;
	DirectX::XMMATRIX GetAppliedTransform() const noexcept;
	void SetAppliedTransform(const DirectX::FXMMATRIX& appTrans) noexcept;
	DirectX::XMMATRIX GetBaseTransform() const noexcept;
	int GetId() const;
	
	
private:
	void AddChild(std::unique_ptr<Node> node_in);
	void ShowTree(Node*& eslectedNode) const noxnd;
private:
	std::vector<Mesh*> meshes;
	std::vector<std::unique_ptr<Node>> children;
	DirectX::XMFLOAT4X4 transform;
	DirectX::XMFLOAT4X4 appliedTransform;
	std::string name;
	int id;
};


class Model
{
	
public:
	Model(Graphics& gfx, const char* filename);
	~Model() noexcept;
private:
	void ParseMesh(const aiMesh* mesh_in, float scale);
	
	std::unique_ptr<Node> ParseNode(int& node_id, aiNode* node_in);
	
	
public :
	void Draw();
	void ShowWindow(const char* windowName) noexcept;
	
private:
	std::vector<std::unique_ptr<Mesh>> meshes;
	std::unique_ptr<Node> pRoot;
	Graphics& gfx;
	std::unique_ptr<class WindowMenu> pWindow;
};

