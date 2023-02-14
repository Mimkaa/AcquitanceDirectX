#include "Node.h"
#include <string>
#include "GraphicsThrowHeader.h"
#include "Mesh.h"
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include "Graphics.hpp"
#include "imgui/imgui.h"

namespace dx = DirectX;
Node::Node(int id_in, const std::string& name_in, std::vector<Mesh*> meshes_in, const DirectX::XMMATRIX& transfomation)
	:
	meshes(std::move(meshes_in)),
	name(name_in),
	id(id_in)

{
	DirectX::XMStoreFloat4x4(&transform, transfomation);
	DirectX::XMStoreFloat4x4(&appliedTransform, DirectX::XMMatrixIdentity());
}

int Node::GetId() const
{
	return id;
}

DirectX::XMMATRIX Node::GetAppliedTransform() const noexcept
{

	return DirectX::XMLoadFloat4x4(&appliedTransform);
}

void Node::SetAppliedTransform(const DirectX::FXMMATRIX& appTrans) noexcept
{
	DirectX::XMStoreFloat4x4(&appliedTransform, appTrans);
}

DirectX::XMMATRIX Node::GetBaseTransform() const noexcept
{
	return DirectX::XMLoadFloat4x4(&transform);
}



//void Node::ShowTree(Node*& selectedNode) const noxnd
//{
//	// recursively increment the index of a node
//	const int currentNodeId = (selectedNode == nullptr) ? -1 : selectedNode->GetId();
//	// build up flags for current node
//	const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow
//		| ((GetId() == currentNodeId) ? ImGuiTreeNodeFlags_Selected : 0)
//		| ((children.size() == 0) ? ImGuiTreeNodeFlags_Leaf : 0);
//	// if tree node expanded, recursively render all children
//	const auto expanded = ImGui::TreeNodeEx((void*)(intptr_t)GetId(), node_flags, name.c_str());
//	if (ImGui::IsItemClicked())
//	{
//		selectedNode = const_cast<Node*>(this);
//	}
//	if (expanded) {
//
//
//		for (const auto& child : children)
//			child->ShowTree(selectedNode);
//		ImGui::TreePop();
//	}
//}

void Node::Submit(FrameComander& frame, DirectX::FXMMATRIX accumulatedTransform) const noxnd
{
	const auto built =
		dx::XMLoadFloat4x4(&appliedTransform) *
		dx::XMLoadFloat4x4(&transform) *
		accumulatedTransform;
	for (const auto pm : meshes)
	{
		pm->Submit(frame, accumulatedTransform);
	}
	for (const auto& pc : children)
	{
		pc->Submit(frame, accumulatedTransform);
	}
}

void Node::AddChild(std::unique_ptr<Node> node_in)
{
	assert(node_in);
	children.push_back(std::move(node_in));
}