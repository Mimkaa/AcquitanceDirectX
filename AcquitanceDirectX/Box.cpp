#include "Box.h"
#include "BindableBase.h"
#include "GraphicsThrowHeader.h"
#include "Cube.h"
#include "imgui/imgui.h"
#include <format>
Box::Box(Graphics& gfx, std::mt19937& rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist,
	std::uniform_real_distribution<float>& bdist,
	DirectX::XMFLOAT3 mat)
	:
	TestObj(gfx, rng, adist, ddist, odist, rdist)
	
{
	if (!IsStaticInitialized())
	{
		namespace dx = DirectX;

		struct Vertex
		{
			dx::XMFLOAT3 pos;
			dx::XMFLOAT3 n;
		};
		
		auto model = Cube::MakeIndependent<Vertex>();
		model.SetNormalsIndependentFlat();

		model.Transform(dx::XMMatrixScaling(1.0f, 1.0f, 1.2f));

		AddStaticBind(std::make_unique<VertexBuffer>(gfx, model.vertices));

		


		auto pvs = std::make_unique<VertexShader>(gfx, L"PhongVS.cso");
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<PixelShader>(gfx, L"PhongPS.cso"));

		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.indices));

		
		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{"Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
			{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};
		AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));
		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else
	{
		SetIndexFromStatic();
	}
	AddBind(std::make_unique<TransformCbuf>(gfx, *this,1u));


	materialConstants.color = mat;
	AddBind(std::make_unique<MaterialCBuf>(gfx, materialConstants, 1u));

	// model deformation transform (per instance, not stored as bind)
	DirectX::XMStoreFloat3x3(
		&mt,
		DirectX::XMMatrixScaling(1.0f, 1.0f, bdist(rng))
	);


}



DirectX::XMMATRIX Box::GetTransformXM() const noexcept
{
	namespace dx = DirectX;
	return TestObj::GetTransformXM() * dx::XMLoadFloat3x3(&mt);
}

bool Box::SpawnControlWindow(int id, Graphics& gfx)
{
	bool changed = false;
	bool closed = true;
	if (ImGui::Begin(std::format("Manager For Box number {}", std::to_string(id)).c_str(), &closed))
	{
		auto const mc = ImGui::ColorEdit3("Material Color", &materialConstants.color.x);
		auto const si = ImGui::SliderFloat("SpecularIntensity", &materialConstants.specularIntensity, 0.1f, 4.0f, "%.1f",2);
		auto const sp = ImGui::SliderFloat("SpecularPower", &materialConstants.specularPower, 1.0f, 150.0f, "%.1f",2);
		changed = mc || si || sp;
	}
	if (changed)
	{
		SyncMaterial(gfx);
	}
	
	ImGui::End();
	return closed;
}

void Box::SyncMaterial(Graphics& gfx) noexcept(!IS_DEBUG)
{
	auto pConstB = QueryBindables<MaterialCBuf>();
	assert(pConstB != nullptr);
	pConstB->Update(gfx, materialConstants);
}
