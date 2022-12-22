#include "TestPlane.h"
#include "Plane.h"
#include "BindableCommon.h"
#include "imgui/imgui.h"

TestPlane::TestPlane(Graphics& gfx)
{
	using namespace Bind;
	namespace dx = DirectX;

	auto layout = Dvtx::VertexLayout{}.Append(Dvtx::VertexLayout::Position3D).Append(Dvtx::VertexLayout::Normal).
		Append(Dvtx::VertexLayout::Texture2D);

	auto plane = Plane::Make(std::move(layout));
	std::string TagName = "wall$";
	std::string base = "Models\\brick_wall\\";

	AddBind(Texture::Resolve(gfx, base + "brick_wall_diffuse.jpg", 0));
	AddBind(Texture::Resolve(gfx, base + "brick_wall_normal_rotatedX.jpg", 2));

	AddBind(Sampler::Resolve(gfx));

	AddBind(VertexBuffer::Resolve(gfx, TagName, plane.vertices));
	AddBind(IndexBuffer::Resolve(gfx, TagName, plane.indices));

	auto pvs = VertexShader::Resolve(gfx, "PhongVS.cso");
	auto pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));

	AddBind(PixelShader::Resolve(gfx, "PhongNormalsPS.cso"));

	


	AddBind(PixelConstantBuffer<PSConstant>::Resolve(gfx, Const,1u));



	AddBind(InputLayout::Resolve(gfx, plane.vertices.GetLayout(), pvsbc));

	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));


	AddBind(std::make_shared<Bind::TransformCbufDoubleBoi>(gfx, *this,0, 2));

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

void TestPlane::ShowControlWindow(Graphics& gfx) noexcept
{
	using namespace Bind;
	bool ff = static_cast<bool>(Const.normalsOn);

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
		ImGui::Checkbox("NormalMapping", &ff);
	}
	Const.normalsOn = static_cast<BOOL>(ff);
	PixelConstantBuffer<PSConstant>* pPcbuf = QueryBindables<PixelConstantBuffer<PSConstant>>();
	pPcbuf->Update(gfx, Const);
	pPcbuf->Bind(gfx);
	ImGui::End();

	
}