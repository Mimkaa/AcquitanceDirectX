#include "SolidSphere.h"
#include "BindableCommon.h"
#include "GraphicsThrowHeader.h"
#include "Sphere.h"

SolidSphere::SolidSphere(Graphics& gfx, float radius)
{
	namespace dx = DirectX;

	
	
	auto model = Sphere::Make();
	model.Transform(dx::XMMatrixScaling(radius, radius, radius));
	AddBind(std::make_shared<Bind::VertexBuffer>(gfx, model.vertices));
	AddBind(std::make_shared<Bind::IndexBuffer>(gfx, model.indices));

		

	auto pvs = std::make_shared<Bind::VertexShader>(gfx, "SolidVS.cso");
	auto pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));

	AddBind(std::make_shared<Bind::PixelShader>(gfx, L"SolidPS.cso"));

	struct PSColorConstant
	{
		dx::XMFLOAT3 color = { 1.0f,1.0f,1.0f };
		float padding;
	} colorConst;
		
	AddBind(std::make_shared<Bind::PixelConstantBuffer<PSColorConstant>>(gfx, colorConst));

	
	AddBind(std::make_shared<Bind::InputLayout>(gfx, model.vertices.GetLayout().GetD3DLayout(), pvsbc));

	AddBind(std::make_shared<Bind::Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	

	AddBind(std::make_shared<Bind::TransformCbuf>(gfx, *this));
	

	

	
}


void SolidSphere::SetPos(DirectX::XMFLOAT3 pos) noexcept
{
	this->pos = pos;
}

DirectX::XMMATRIX SolidSphere::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
}