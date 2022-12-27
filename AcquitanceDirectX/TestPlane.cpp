#include "TestPlane.h"
#include "Plane.h"
#include "BindableCommon.h"
#include "imgui/imgui.h"

TestPlane::TestPlane(Graphics& gfx, DirectX::XMFLOAT4 color_in)
	:
	Const({ color_in })
{
	using namespace Bind;
	namespace dx = DirectX;

	auto plane = Plane::Make();
	std::string TagName = "wall$";
	


	AddBind(VertexBuffer::Resolve(gfx, TagName, plane.vertices));
	AddBind(IndexBuffer::Resolve(gfx, TagName, plane.indices));

	

	auto pvs = VertexShader::Resolve(gfx, "SolidVS.cso");
	auto pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));

	AddBind(PixelShader::Resolve(gfx, "SolidPS.cso"));
	AddBind(PixelConstantBuffer<PSConstant>::Resolve(gfx, Const, 1u));

	AddBind(InputLayout::Resolve(gfx, plane.vertices.GetLayout(), pvsbc));

	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddBind(Blender::Resolve(gfx, true, 0.5f));
	AddBind(Rasterizer::Resolve(gfx, true));


	AddBind(std::make_shared<Bind::TransformCbuf>(gfx, *this,0));

	

}

DirectX::XMMATRIX TestPlane::GetTransformXM() const noexcept 
{
	return DirectX::XMMatrixRotationRollPitchYaw(pos.pitch, pos.yaw, pos.roll) * DirectX::XMMatrixTranslation(pos.pos.x, pos.pos.y, pos.pos.z);
}

void TestPlane::SetPos(const DirectX::XMMATRIX mat) noexcept
{
	DirectX::XMVECTOR poss = DirectX::XMLoadFloat3(&pos.pos);

	DirectX::XMVECTOR transPos = DirectX::XMVector3Transform(poss, mat);

	DirectX::XMStoreFloat3(&pos.pos, transPos);
}
void TestPlane::SetPos(const DirectX::XMVECTOR vec) noexcept
{
	DirectX::XMStoreFloat3(&pos.pos, vec);
}

void TestPlane::ShowControlWindow(Graphics& gfx) noexcept
{
	using namespace Bind;
	

	if (ImGui::Begin("PlaneConstrolWindow"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("x_offset", &pos.pos.x, -20.0f, 20.0f);
		ImGui::SliderFloat("y_offset", &pos.pos.y, -20.0f, 20.0f);
		ImGui::SliderFloat("z_offset", &pos.pos.z, -20.0f, 20.0f);
		ImGui::Text("Rotation");
		ImGui::SliderAngle("roll", &pos.roll, -180.0f, 180.0f, "%.2f deg");
		ImGui::SliderAngle("pitch", &pos.pitch, -180.0f, 180.0f, "%.2f deg");
		ImGui::SliderAngle("yaw", &pos.yaw, -180.0f, 180.0f, "%.2f deg");
		const auto pBlender = QueryBindables<Blender>();
		auto blenderFactor = pBlender->GetFactor();
		ImGui::SliderFloat("BlendFactor", &blenderFactor, 0.0f, 1.0f);
		pBlender->SetFactor(blenderFactor);


	}
	
	
	ImGui::End();

	
}