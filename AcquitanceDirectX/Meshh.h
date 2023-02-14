#pragma once
#include "Drawable.h"
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
#include <filesystem>
#include <string>

class Material;


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

class Mesh:public Drawable
{
public:
	Mesh(Graphics& gfx, const Material& mat, const aiMesh& mesh) noexcept;
	//Mesh(Graphics& gfx, std::vector<std::shared_ptr<Bind::Bindable>>binds_in);
	using Drawable::Drawable;
	DirectX::XMMATRIX GetTransformXM() const noexcept;

	void Submit(FrameComander& frame, DirectX::FXMMATRIX accumulatedTranform) const noxnd;
private:
	mutable DirectX::XMFLOAT4X4 transform;
};


class Node
{
	friend class Model;
	friend class WindowMenu;

public:
	struct PSMaterialConstantFullMante
	{
		BOOL specularMapEnabled = TRUE;
		DirectX::XMFLOAT3 specularColor = { 0.3f, 0.8f, 0.7f };
		float specularMapWeight = 0.6f;
		BOOL hasGlossMap;
		float specularPower = 35.0f;
		BOOL  normalMapEnabled = TRUE;
		
	};

	struct PSMaterialNotex
	{
		DirectX::XMFLOAT4 material;
		DirectX::XMFLOAT4 specularColor;
		float specularPower;
		float padding[3];
	} pmc;

public:
	void Submit(FrameComander& frame, DirectX::FXMMATRIX accumulatedTransform) const noxnd;
	Node(int id_in, const std::string& name_in, std::vector<Mesh*> meshes_in, const DirectX::XMMATRIX& transfomation);
	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) noxnd;
	DirectX::XMMATRIX GetAppliedTransform() const noexcept;
	void SetAppliedTransform(const DirectX::FXMMATRIX& appTrans) noexcept;
	DirectX::XMMATRIX GetBaseTransform() const noexcept;
	int GetId() const; 

	template <class C>
	bool ControlMeDaddy(Graphics& gfx, C& pm_in) const noexcept;
	
	
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

	Model(Graphics& gfx, const std::string& pathname, const float scale = 1.0f);
	~Model() noexcept;
private:
	void ParseMesh(const aiMesh* mesh_in, float scale, const aiMaterial* const* ppMaterials, const std::filesystem::path& path);
	
	std::unique_ptr<Node> ParseNode(int& node_id, aiNode* node_in);
	
	
public :
	void Submit(FrameComander& frame) const noxnd;
	void SetRootTransform(const DirectX::FXMMATRIX& rt) noexcept;
	void Draw();
	void ShowWindow(Graphics& gfx, const char* windowName) noexcept;
	
private:
	std::vector<std::unique_ptr<Mesh>> meshes;
	std::unique_ptr<Node> pRoot;
	Graphics& gfx;
	std::unique_ptr<class WindowMenu> pWindow;
};

