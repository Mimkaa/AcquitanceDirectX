#include "Mesh.h"
#include <unordered_map>
#include <sstream>
#include <type_traits>
#include "Surface.h"
// mesh
Mesh::Mesh(Graphics& gfx, std::vector<std::shared_ptr<Bind::Bindable>>binds_in)
{
	
	AddBind(Bind::Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	
	for (auto& b : binds_in)
	{
		
		AddBind(std::move(b));
		
	}

	AddBind(std::make_shared<Bind::TransformCbuf>(gfx, *this));
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

void Node::ShowTree(Node*& selectedNode) const noxnd
{
	// recursively increment the index of a node
	const int currentNodeId = (selectedNode == nullptr)? -1: selectedNode->GetId();
	// build up flags for current node
	const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow
		| ((GetId() == currentNodeId) ? ImGuiTreeNodeFlags_Selected : 0)
		| ((children.size() == 0) ? ImGuiTreeNodeFlags_Leaf : 0);
	// if tree node expanded, recursively render all children
	const auto expanded = ImGui::TreeNodeEx((void*)(intptr_t)GetId(), node_flags, name.c_str());
	if (ImGui::IsItemClicked())
	{
		selectedNode = const_cast<Node*>(this);
	}
	if (expanded) {

		
		for (const auto& child : children)
			child->ShowTree(selectedNode);
		ImGui::TreePop();
	}
}

void Node::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform)noxnd
{
	const auto build =   XMLoadFloat4x4(&appliedTransform) * XMLoadFloat4x4(&transform) * accumulatedTransform;
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

template <class C>
bool Node::ControlMeDaddy(Graphics& gfx, C& c) const noexcept
{
	
	if constexpr (std::is_same<C, Node::PSMaterialConstantFullMante>::value)
	{
		if (!meshes.empty())
		{
			if (auto pcb = meshes.front()->QueryBindables<Bind::PixelConstantBuffer<C>>())
			{
				ImGui::Text("Material");

				bool normalMapEnabled = (bool)c.normalMapEnabled;
				ImGui::Checkbox("Norm Map", &normalMapEnabled);
				c.normalMapEnabled = normalMapEnabled ? TRUE : FALSE;

				bool specularMapEnabled = (bool)c.specularMapEnabled;
				ImGui::Checkbox("Spec Map", &specularMapEnabled);
				c.specularMapEnabled = specularMapEnabled ? TRUE : FALSE;

				bool hasGlossMap = (bool)c.hasGlossMap;
				ImGui::Checkbox("Gloss Alpha", &hasGlossMap);
				c.hasGlossMap = hasGlossMap ? TRUE : FALSE;

				ImGui::SliderFloat("Spec Weight", &c.specularMapWeight, 0.0f, 2.0f);

				ImGui::SliderFloat("Spec Pow", &c.specularPower, 0.0f, 1000.0f, "%f", 5.0f);

				ImGui::ColorPicker3("Spec Color", reinterpret_cast<float*>(&c.specularColor));

				pcb->Update(gfx, c);
				return true;
			}
		}
		
	}
	else if constexpr (std::is_same<C, Node::PSMaterialNotex>::value)
	{
		if (!meshes.empty()) {
			if (auto pcb = meshes.front()->QueryBindables<Bind::PixelConstantBuffer<C>>())
			{
				ImGui::Text("Material");
				ImGui::ColorPicker3("Spec Color", reinterpret_cast<float*>(&c.specularColor));

				float specularPower = c.specularPower;
				ImGui::SliderFloat("Spec Pow", &specularPower, 0.0f, 1000.0f, "%f", 5.0f);
				c.specularPower = specularPower;

				DirectX::XMFLOAT4 mat= c.material;
				ImGui::ColorPicker3("Diff Color", reinterpret_cast<float*>(&mat));
				c.material = mat;

				pcb->Update(gfx, c);
				return true;
			}
		}
	
	}
	return false;
}


// model
class WindowMenu//pImpl Idiom : class whose implementation can be found only in this cpp
{

public:
	struct Transformations {
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
		float roll = 0.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;
	};

	void Show(Graphics& gfx, const char* windowName, const Node& root)
	{
	
		windowName = windowName ? windowName : "Model";

		if (ImGui::Begin(windowName))
		{
			ImGui::Columns(2, nullptr, false);
			root.ShowTree(selectedNode);
			ImGui::NextColumn();

			if (selectedNode)
			{
				auto& transform = transforms[selectedNode->GetId()];
				ImGui::Text("Position");
				ImGui::SliderFloat("x_offset", &transform.x, -20.0f, 20.0f);
				ImGui::SliderFloat("y_offset", &transform.y, -20.0f, 20.0f);
				ImGui::SliderFloat("z_offset", &transform.z, -20.0f, 20.0f);
				ImGui::Text("Rotation");
				ImGui::SliderAngle("roll", &transform.roll, -180.0f, 180.0f, "%.2f deg");
				ImGui::SliderAngle("pitch", &transform.pitch, -180.0f, 180.0f, "%.2f deg");
				ImGui::SliderAngle("yaw", &transform.yaw, -180.0f, 180.0f, "%.2f deg");
				selectedNode->SetAppliedTransform(GetTransformation(transform));
				if (!selectedNode->ControlMeDaddy<Node::PSMaterialConstantFullMante>(gfx, pm))
				{
					selectedNode->ControlMeDaddy<Node::PSMaterialNotex>(gfx, pmn);
				}
			}
			ImGui::Columns();
		}

		ImGui::End();
	}
	DirectX::XMMATRIX GetTransformation(const Transformations& pos) const noexcept
	{
		return DirectX::XMMatrixRotationRollPitchYaw(pos.pitch, pos.yaw, pos.roll) *
			DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
	}

private:
	
	Node* selectedNode = nullptr;
	Node::PSMaterialConstantFullMante pm;
	Node::PSMaterialNotex pmn;
	std::unordered_map<int, Transformations> transforms;
};

Model::Model(Graphics& gfx, const char* filename)
	:
	gfx(gfx),
	pWindow(std::make_unique<WindowMenu>())
{
	Assimp::Importer importer;
	const auto scene = importer.ReadFile(filename,
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace
	);
	if (!scene)
	{
		throw ModelException(__LINE__,__FILE__, importer.GetErrorString());
	}
	for (int i = 0; i < scene->mNumMeshes; i++)
	{
		ParseMesh(scene->mMeshes[i], 1.0f, scene->mMaterials);
	}
	int firstNodeId = 0;
	pRoot = ParseNode(firstNodeId, scene->mRootNode);
	
}

Model::~Model() noexcept
{
}

void Model::ParseMesh(const aiMesh* mesh_in, float scale, const aiMaterial*const* ppMaterials)
{
	using namespace Bind;
	namespace dx = DirectX;
	using namespace std::literals::string_literals;
	bool hasSppecularMap = false;
	bool hasNormalMap = false;
	bool hasDiffuseMap = false;
	bool hasAlphaGloss = false;

	std::vector<std::shared_ptr<Bind::Bindable>> currBinds;
	using namespace std::string_literals;
	std::string base = "Models\\gobber\\"s;
	const std::string meshTag = base + "%" + mesh_in->mName.C_Str();
	float shininess = 35.0f;

	dx::XMFLOAT4 diffuseColor = { 0.65f,0.65f,0.85f,1.0f };
	dx::XMFLOAT4 specularColor = { 0.7f, 0.8f, 0.7f, 1.0f };

	if (mesh_in->mMaterialIndex >= 0)
	{
		auto& material = *ppMaterials[mesh_in->mMaterialIndex];
		aiString textureSrc;
	
		
		if (material.GetTexture(aiTextureType_SPECULAR, 0, &textureSrc) == aiReturn_SUCCESS)
		{
			hasSppecularMap = true;
			auto tex = Texture::Resolve(gfx, base + textureSrc.C_Str(), 1);
			hasAlphaGloss = tex->HasAlpha();
			currBinds.push_back(std::move(tex));

		}
		else
		{
			material.Get(AI_MATKEY_COLOR_SPECULAR, reinterpret_cast<aiColor4D&>(diffuseColor));
		}
		if(!hasAlphaGloss)
		{
			material.Get(AI_MATKEY_SHININESS, shininess);
		}


		if (material.GetTexture(aiTextureType_NORMALS, 0, &textureSrc) == aiReturn_SUCCESS)
		{
			hasNormalMap = true;
			currBinds.push_back(Texture::Resolve(gfx, base + textureSrc.C_Str(), 2));
		}
		if (material.GetTexture(aiTextureType_DIFFUSE, 0, &textureSrc) == aiReturn_SUCCESS)
		{
			hasDiffuseMap = true;
			currBinds.push_back(Texture::Resolve(gfx, base + textureSrc.C_Str(), 0));
		}
		else
		{
			material.Get(AI_MATKEY_COLOR_DIFFUSE, reinterpret_cast<aiColor4D&>(specularColor));
		}
		

		if (hasDiffuseMap || hasNormalMap || hasSppecularMap)
		{
			currBinds.push_back(Sampler::Resolve(gfx));
		}
		
	}
	
	
	// different bindables for different cases
	// Jesus Crist you are stupid as hell not all meshes of the model have the same number of textures
	// so that some of them for example have specular some of them do not so basically you have to go through all the cases

	
	if (hasDiffuseMap && hasNormalMap && hasSppecularMap)
	{
		using Dvtx::VertexLayout;
		Dvtx::VertexBuffer vb{ std::move(VertexLayout{}
		.Append(VertexLayout::Position3D)
		.Append(VertexLayout::Normal)
		.Append(VertexLayout::Texture2D))
		.Append((VertexLayout::Tangent))
		.Append((VertexLayout::Bytangent)) };

		for (int i = 0; i < mesh_in->mNumVertices; i++)
		{
			vb.EmplaceBack(
				dx::XMFLOAT3{ mesh_in->mVertices[i].x * scale,mesh_in->mVertices[i].y * scale ,mesh_in->mVertices[i].z * scale },
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh_in->mNormals[i]),
				*reinterpret_cast<DirectX::XMFLOAT2*>(&mesh_in->mTextureCoords[0][i]),
				*reinterpret_cast<dx::XMFLOAT3*>(&mesh_in->mTangents[i]),
				*reinterpret_cast<dx::XMFLOAT3*>(&mesh_in->mBitangents[i])
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

		currBinds.push_back(VertexBuffer::Resolve(gfx, meshTag, vb));

		currBinds.push_back(IndexBuffer::Resolve(gfx, meshTag, indices));
		
		auto pvs = VertexShader::Resolve(gfx, "PhongSpecNormalVS.cso");
		auto pvsbc = pvs->GetBytecode();
		currBinds.push_back(std::move(pvs));

		
		
		currBinds.push_back(PixelShader::Resolve(gfx, "PhongSpecNormalPS.cso"));
		
		Node::PSMaterialConstantFullMante pmc;
		pmc.specularPower = shininess;
		pmc.hasGlossMap = hasAlphaGloss ? TRUE : FALSE;

		currBinds.push_back(PixelConstantBuffer<Node::PSMaterialConstantFullMante>::Resolve(gfx, pmc, 1u));


		currBinds.push_back(InputLayout::Resolve(gfx, vb.GetLayout(), pvsbc));
		

	}
	else if (hasDiffuseMap && hasNormalMap)
	{
		using Dvtx::VertexLayout;
		Dvtx::VertexBuffer vb{ std::move(VertexLayout{}
		.Append(VertexLayout::Position3D)
		.Append(VertexLayout::Normal)
		.Append(VertexLayout::Texture2D))
		.Append((VertexLayout::Tangent))
		.Append((VertexLayout::Bytangent)) };

		for (int i = 0; i < mesh_in->mNumVertices; i++)
		{
			vb.EmplaceBack(
				dx::XMFLOAT3{ mesh_in->mVertices[i].x * scale,mesh_in->mVertices[i].y * scale ,mesh_in->mVertices[i].z * scale },
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh_in->mNormals[i]),
				*reinterpret_cast<DirectX::XMFLOAT2*>(&mesh_in->mTextureCoords[0][i]),
				*reinterpret_cast<dx::XMFLOAT3*>(&mesh_in->mTangents[i]),
				*reinterpret_cast<dx::XMFLOAT3*>(&mesh_in->mBitangents[i])
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

	


		currBinds.push_back(VertexBuffer::Resolve(gfx, meshTag, vb));

		currBinds.push_back(IndexBuffer::Resolve(gfx, meshTag, indices));
		// no specular mapping
		auto pvs = VertexShader::Resolve(gfx, "PhongNormalsVS.cso");
		auto pvsbc = pvs->GetBytecode();
		currBinds.push_back(std::move(pvs));

		currBinds.push_back(PixelShader::Resolve(gfx, "PhongNormalsPS.cso"));

		struct PSMaterialConstant
		{
			float specularIntensity = 0.18f;
			float specularPower;
			float padding[2];
		} pmc;
		pmc.specularPower = shininess;

		currBinds.push_back(PixelConstantBuffer<PSMaterialConstant>::Resolve(gfx, pmc, 1u));

		currBinds.push_back(InputLayout::Resolve(gfx, vb.GetLayout(), pvsbc));
	}
	else if (hasDiffuseMap)
	{ 
		using Dvtx::VertexLayout;
		Dvtx::VertexBuffer vb{ std::move(VertexLayout{}
		.Append(VertexLayout::Position3D)
		.Append(VertexLayout::Normal)
		.Append(VertexLayout::Texture2D))
		};

		for (int i = 0; i < mesh_in->mNumVertices; i++)
		{
			vb.EmplaceBack(
				dx::XMFLOAT3{ mesh_in->mVertices[i].x * scale,mesh_in->mVertices[i].y * scale ,mesh_in->mVertices[i].z * scale },
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh_in->mNormals[i]),
				*reinterpret_cast<DirectX::XMFLOAT2*>(&mesh_in->mTextureCoords[0][i])
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

		


		currBinds.push_back(VertexBuffer::Resolve(gfx, meshTag, vb));

		currBinds.push_back(IndexBuffer::Resolve(gfx, meshTag, indices));
		// no specular mapping
		auto pvs = VertexShader::Resolve(gfx, "PhongVS.cso");
		auto pvsbc = pvs->GetBytecode();
		currBinds.push_back(std::move(pvs));

		currBinds.push_back(PixelShader::Resolve(gfx, "PhongPS.cso"));

		currBinds.push_back(InputLayout::Resolve(gfx, vb.GetLayout(), pvsbc));

		struct PSMaterialConstantDiffuse
		{

			float specularIntensity = 0.18f;
			float specularPower;
			float padding[2];
		} pmc;
		pmc.specularPower = shininess;
		currBinds.push_back(PixelConstantBuffer<PSMaterialConstantDiffuse>::Resolve(gfx, pmc, 1u));

		
	}
	else if (!hasDiffuseMap && !hasNormalMap && !hasSppecularMap)
	{

	using Dvtx::VertexLayout;
	Dvtx::VertexBuffer vb{ std::move(VertexLayout{}
	.Append(VertexLayout::Position3D)
	.Append(VertexLayout::Normal))
	};

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




	currBinds.push_back(VertexBuffer::Resolve(gfx, meshTag, vb));

	currBinds.push_back(IndexBuffer::Resolve(gfx, meshTag, indices));
	// no specular mapping
	auto pvs = VertexShader::Resolve(gfx, "PhongNotexVS.cso");
	auto pvsbc = pvs->GetBytecode();
	currBinds.push_back(std::move(pvs));

	currBinds.push_back(PixelShader::Resolve(gfx, "PhongNotexPS.cso"));

	currBinds.push_back(InputLayout::Resolve(gfx, vb.GetLayout(), pvsbc));

	
	Node::PSMaterialNotex pmc;
	pmc.material = diffuseColor;
	pmc.specularColor = specularColor;
	pmc.specularPower = shininess;
	currBinds.push_back(PixelConstantBuffer<Node::PSMaterialNotex>::Resolve(gfx, pmc, 1u));

	}
	
	
	meshes.push_back(std::make_unique<Mesh>(gfx, std::move(currBinds)));

	

}

void Model::Draw()
{
	
	pRoot->Draw(gfx, pRoot->GetAppliedTransform() * pRoot->GetBaseTransform());
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
		pNode->AddChild(ParseNode(node_id,node_in->mChildren[i]));
	}
	return pNode;
}


ModelException::ModelException(int line, const char* file, std::string note) noexcept
	:
	ChiliException(line, file),
	note(note)
{
}

const char* ModelException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Origin]:" << GetOriginString()<< std::endl
		<< "[Message]"<<GetNote() << std::endl;
	whatBuffer = oss.str();
	return whatBuffer.c_str();

}

// exception
const std::string& ModelException::GetNote() const noexcept
{
	return note;
}

const char* ModelException::GetType() const noexcept
{
	return "Chilli Model Exception";
}
