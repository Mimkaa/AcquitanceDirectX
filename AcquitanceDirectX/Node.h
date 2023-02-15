#pragma once
#include <memory>
#include <vector>
#include "Graphics.hpp"

class Model;
class Mesh;
class FrameComander;
class ModelProbe;

class Node
{
	friend class Model;
	friend class WindowMenu;


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
	bool HasChildren() const noexcept
	{
		return children.size() > 0;
	}
	void Accept(class ModelProbe& probe);
	const std::string& GetName() const
	{
		return name;
	}


private:
	void AddChild(std::unique_ptr<Node> node_in);
	//void ShowTree(Node*& eslectedNode) const noxnd;
private:
	std::vector<Mesh*> meshes;
	std::vector<std::unique_ptr<Node>> children;
	DirectX::XMFLOAT4X4 transform;
	DirectX::XMFLOAT4X4 appliedTransform;
	std::string name;
	int id;
};