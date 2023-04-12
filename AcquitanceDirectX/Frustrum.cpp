#include "Frustrum.h"
#include "Graphics.hpp"
#include "BindableCommon.h"
#include <string>

Frustrum::Frustrum(Graphics& gfx, float width, float height, float fFar, float fNear) noexcept
{

	std::string tag = "fruc";
	std::vector<unsigned short> indices;

	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(2);
	indices.push_back(3);
	indices.push_back(3);
	indices.push_back(0);
	indices.push_back(4);
	indices.push_back(5);
	indices.push_back(5);
	indices.push_back(6);
	indices.push_back(6);
	indices.push_back(7);
	indices.push_back(7);
	indices.push_back(4);

	indices.push_back(0);
	indices.push_back(4);
	indices.push_back(1);
	indices.push_back(5);
	indices.push_back(2);
	indices.push_back(6);
	indices.push_back(3);
	indices.push_back(7);

	pIndexBuff = Bind::IndexBuffer::Resolve(gfx, tag, indices);

	pTopology = Bind::Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	Technique line;
	SetVertBuffer(gfx, width, height, fFar, fNear);
	{
		
		Step only("Lambertian");
		auto pvs = Bind::VertexShader::Resolve(gfx, "Solid_VS.cso");
		auto pvsbc = pvs->GetBytecode();
		only.AddBindable(Bind::InputLayout::Resolve(gfx,pVertBuff->GetLayout(), *pvs));
		only.AddBindable(std::move(pvs));

		only.AddBindable(Bind::PixelShader::Resolve(gfx, "Solid_PS.cso"));

		struct PSColorConstant
		{
			DirectX::XMFLOAT3 color = { 0.8f,0.2f,0.2f };
			float padding;
		} colorConst;
		only.AddBindable(Bind::PixelConstantBuffer<PSColorConstant>::Resolve(gfx, colorConst, 1u));



		only.AddBindable(std::make_shared<Bind::TransformCbuf>(gfx));

		only.AddBindable(Bind::Blender::Resolve(gfx, false));

		only.AddBindable(Bind::Rasterizer::Resolve(gfx, false));

		line.AddStep(std::move(only));
		
	}
	{
		Step only("wireframe");
		auto pvs = Bind::VertexShader::Resolve(gfx, "Solid_VS.cso");
		auto pvsbc = pvs->GetBytecode();
		only.AddBindable(Bind::InputLayout::Resolve(gfx, pVertBuff->GetLayout(), *pvs));
		only.AddBindable(std::move(pvs));

		only.AddBindable(Bind::PixelShader::Resolve(gfx, "Solid_PS.cso"));

		struct PSColorConstant
		{
			DirectX::XMFLOAT3 color = { 1.0f,1.0f,1.0f };
			float padding;
		} colorConst;
		only.AddBindable(Bind::PixelConstantBuffer<PSColorConstant>::Resolve(gfx, colorConst, 1u));



		only.AddBindable(std::make_shared<Bind::TransformCbuf>(gfx));

		only.AddBindable(Bind::Blender::Resolve(gfx, false));

		only.AddBindable(Bind::Rasterizer::Resolve(gfx, false));

		line.AddStep(std::move(only));
		
	}
	AddTechnique(std::move(line));
}

void Frustrum::SetVertBuffer(Graphics& gfx, float width, float height, float fFar, float fNear)
{
	float ratio = fFar / fNear;
	float xNear = width / 2;
	float yNear = height / 2;
	float xFar = xNear * ratio;
	float yFar = yNear * ratio;

	Dvtx::VertexLayout vl;
	vl.Append(Dvtx::VertexLayout::Position3D);
	Dvtx::VertexBuffer vb{ vl };

	vb.EmplaceBack(DirectX::XMFLOAT3{ -xNear, yNear, fNear });
	vb.EmplaceBack(DirectX::XMFLOAT3{ xNear, yNear, fNear });
	vb.EmplaceBack(DirectX::XMFLOAT3{ xNear, -yNear, fNear });
	vb.EmplaceBack(DirectX::XMFLOAT3{ -xNear, -yNear, fNear });
	vb.EmplaceBack(DirectX::XMFLOAT3{ -xFar, yFar, fFar });
	vb.EmplaceBack(DirectX::XMFLOAT3{ xFar, yFar, fFar });
	vb.EmplaceBack(DirectX::XMFLOAT3{ xFar, -yFar, fFar });
	vb.EmplaceBack(DirectX::XMFLOAT3{ -xFar, -yFar, fFar });

	pVertBuff = std::make_shared<Bind::VertexBuffer>(gfx, vb);


}


void Frustrum::SetPosition(DirectX::XMFLOAT3 pos) noexcept
{
	this->pos = pos;
}


void Frustrum::SetRotation(DirectX::XMFLOAT3 rot) noexcept
{
	this->rot = rot;
}


DirectX::XMMATRIX Frustrum::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z)
		* DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
}