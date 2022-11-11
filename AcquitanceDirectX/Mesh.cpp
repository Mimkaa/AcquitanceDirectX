#include "Mesh.h"
// mesh
Mesh::Mesh(Graphics& gfx, std::vector<std::unique_ptr<Bind::Bindable>>binds_in)
{
	if (!IsStaticInitialized())
	{
		AddStaticBind(std::make_unique<Bind::Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	for (auto& b : binds_in)
	{
		if (auto ptr = dynamic_cast<Bind::IndexBuffer*>(b.get()))
		{
			AddIndexBuffer(std::unique_ptr<Bind::IndexBuffer>(ptr));
			b.release();
		}
		else
		{
			AddBind(std::move(b));
		}
	}
	AddBind(std::make_unique<Bind::TransformCbuf>(gfx, *this, 1u));
}

DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept
{
	return XMLoadFloat4x4(&transform);
}

void Mesh::Draw(Graphics& gfx, const DirectX::FXMMATRIX& accumulatedTransform) const noxnd
{
	DirectX::XMStoreFloat4x4(&transform, accumulatedTransform);
	Drawable::Draw(gfx);
}

// node
Node::Node(const std::string& name_in, std::vector<Mesh*> meshes_in, const DirectX::XMMATRIX& transfomation)
	:
	meshes(std::move(meshes_in)),
	name(name_in)

{
	DirectX::XMStoreFloat4x4(&transform, transfomation);
}

void Node::ShowTree(int& nodeIndex, std::optional<int>& selectedIndex) const noxnd
{
	// recursively increment the index of a node
	const int currentNodeIndex = nodeIndex;
	nodeIndex++;
	// build up flags for current node
	const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow
		| ((currentNodeIndex == selectedIndex.value_or(-1)) ? ImGuiTreeNodeFlags_Selected : 0)
		| ((children.size() == 0) ? ImGuiTreeNodeFlags_Leaf : 0);
	// if tree node expanded, recursively render all children
	if (ImGui::TreeNodeEx((void*)(intptr_t)currentNodeIndex, node_flags, name.c_str())) {

		selectedIndex = ImGui::IsItemClicked() ? currentNodeIndex : selectedIndex;

		for (const auto& child : children)
			child->ShowTree(nodeIndex, selectedIndex);
		ImGui::TreePop();
	}
}

void Node::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform)noxnd
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

void Node::AddChild(std::unique_ptr<Node> node_in)
{
		assert(node_in);
		children.push_back(std::move(node_in));
}

// model
class Window//pImpl Idiom : class whose implementation can be found only in this cpp
{
public:
	void Show(const char* windowName, const Node& root)
	{
	
		windowName = windowName ? windowName : "Model";

		int nodeIndexTracker = 0;
		if (ImGui::Begin(windowName))
		{
			ImGui::Columns(2, nullptr, false);
			root.ShowTree(nodeIndexTracker, selectedIndex);
			ImGui::NextColumn();


			ImGui::Text("Position");
			ImGui::SliderFloat("x_offset", &pos.x, -20.0f, 20.0f);
			ImGui::SliderFloat("y_offset", &pos.y, -20.0f, 20.0f);
			ImGui::SliderFloat("z_offset", &pos.z, -20.0f, 20.0f);
			ImGui::Text("Rotation");
			ImGui::SliderAngle("roll", &pos.roll, -180.0f, 180.0f, "%.2f deg");
			ImGui::SliderAngle("pitch", &pos.pitch, -180.0f, 180.0f, "%.2f deg");
			ImGui::SliderAngle("yaw", &pos.yaw, -180.0f, 180.0f, "%.2f deg");



			ImGui::Columns();
		}

		ImGui::End();
	}
	DirectX::XMMATRIX GetTransformation() const noexcept
	{
		return DirectX::XMMatrixRotationRollPitchYaw(pos.pitch, pos.yaw, pos.roll) *
			DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
	}

private:
	std::optional<int> selectedIndex;
	struct {
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
		float roll = 0.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;
	}pos;
};

Model::Model(Graphics& gfx, const char* filename)
	:
	gfx(gfx),
	pWindow(std::make_unique<Window>())
{
	Assimp::Importer importer;
	const auto scene = importer.ReadFile(filename,
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices);
	for (int i = 0; i < scene->mNumMeshes; i++)
	{
		ParseMesh(scene->mMeshes[i], 1.0f);
	}
	pRoot = ParseNode(scene->mRootNode);
}

Model::~Model() noexcept
{
}

void Model::ParseMesh(const aiMesh* mesh_in, float scale)
{
	namespace dx = DirectX;
	std::vector<std::unique_ptr<Bind::Bindable>> currBinds;

	using Dvtx::VertexLayout;
	Dvtx::VertexBuffer vb{ std::move(VertexLayout{}
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
	currBinds.push_back(std::make_unique<Bind::VertexBuffer>(gfx, vb));

	currBinds.push_back(std::make_unique<Bind::IndexBuffer>(gfx, indices));

	auto pvs = std::make_unique<Bind::VertexShader>(gfx, L"PhongVS.cso");
	auto pvsbc = pvs->GetBytecode();
	currBinds.push_back(std::move(pvs));

	currBinds.push_back(std::make_unique<Bind::PixelShader>(gfx, L"PhongPS.cso"));




	currBinds.push_back(std::make_unique<Bind::InputLayout>(gfx, vb.GetLayout().GetD3DLayout(), pvsbc));

	struct PSMaterialConstants
	{
		DirectX::XMFLOAT3 color = { 0.6f,0.6f,0.8f };
		float specularIntensity = 0.6f;
		float specularPower = 30.0f;
		float padding[3];

	} materialConstants;

	currBinds.push_back(std::make_unique<Bind::PixelConstantBuffer<PSMaterialConstants>>(gfx, materialConstants, 1u));

	meshes.push_back(std::make_unique<Mesh>(gfx, std::move(currBinds)));

}

void Model::Draw()
{
	
	pRoot->Draw(gfx, pWindow->GetTransformation());
}

void Model::ShowWindow(const char* windowName) noexcept
{
	pWindow->Show(windowName, *pRoot);
}



std::unique_ptr<Node> Model::ParseNode(aiNode* node_in)
{
	const auto transform = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(&node_in->mTransformation)));

	std::vector<Mesh*> nodeMeshes;
	nodeMeshes.reserve(node_in->mNumMeshes);
	for (int i = 0; i < node_in->mNumMeshes; i++)
	{
		const auto mesh_index = node_in->mMeshes[i];
		nodeMeshes.push_back(meshes.at(mesh_index).get());
	}
	auto pNode = std::make_unique<Node>(node_in->mName.C_Str(), std::move(nodeMeshes), transform);
	for (int i = 0; i < node_in->mNumChildren; i++)
	{
		pNode->AddChild(ParseNode(node_in->mChildren[i]));
	}
	return pNode;
}



