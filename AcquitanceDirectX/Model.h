#pragma once
#include <memory>
#include <string>
#include <vector>
#include <filesystem>
#include "Graphics.hpp"
#include "ModelWindow.h"

class aiMesh;
class aiMaterial;
class aiNode;
class Node;
class FrameComander;
class Mesh;
 
class Model
{

public:

	Model(Graphics& gfx, const std::string& pathname, const float scale = 1.0f);
	~Model() noexcept;
private:
	void ParseMesh(const aiMesh* mesh_in, float scale, const aiMaterial* const* ppMaterials, const std::filesystem::path& path);

	std::unique_ptr<Node> ParseNode(int& node_id, aiNode* node_in);


public:
	void Submit(FrameComander& frame) const noxnd;
	void SetRootTransform(const DirectX::FXMMATRIX& rt) noexcept;
	void Draw();
	void ShowWindow(Graphics& gfx, const char* windowName) noexcept;

private:
	std::vector<std::unique_ptr<Mesh>> meshes;
	std::unique_ptr<Node> pRoot;
	Graphics& gfx;
	std::unique_ptr<ModelWindow> pWindow;
};