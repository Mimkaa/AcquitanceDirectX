#include "CameraIndicator.h"
#include "Vertex.h"
#include "BindableCommon.h"
#include "Channels.h"

CameraIndicator::CameraIndicator(Graphics& gfx)noexcept
{
	std::string tag = "cam";
	Dvtx::VertexLayout vl;
	vl.Append(Dvtx::VertexLayout::Position3D);
	Dvtx::VertexBuffer vb{vl};

	float x = 1.0f;
	float y = 0.75f;
	float dirIndX = x / 2;
	float ditIndY = y * 1.1;


	vb.EmplaceBack(DirectX::XMFLOAT3{ -x, y, 0.0f });
	vb.EmplaceBack(DirectX::XMFLOAT3{ x, y, 0.0f });
	vb.EmplaceBack(DirectX::XMFLOAT3{ x, -y, 0.0f });
	vb.EmplaceBack(DirectX::XMFLOAT3{ -x, -y, 0.0f });
	vb.EmplaceBack(DirectX::XMFLOAT3{ 0.0f, 0.0f, -2.0f });
	vb.EmplaceBack(DirectX::XMFLOAT3{ -dirIndX, ditIndY , 0.0f });
	vb.EmplaceBack(DirectX::XMFLOAT3{ dirIndX, ditIndY , 0.0f });
	vb.EmplaceBack(DirectX::XMFLOAT3{ 0.0f, ditIndY+y , 0.0f });

	pVertBuff = Bind::VertexBuffer::Resolve(gfx, tag, vb);

	std::vector<unsigned short> indices;

	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(2);
	indices.push_back(3);
	indices.push_back(3);
	indices.push_back(0);
	indices.push_back(0);
	indices.push_back(4);
	indices.push_back(1);
	indices.push_back(4);
	indices.push_back(3);
	indices.push_back(4);
	indices.push_back(2);
	indices.push_back(4);

	indices.push_back(5);
	indices.push_back(6);
	indices.push_back(6);
	indices.push_back(7);
	indices.push_back(7);
	indices.push_back(5);

	pIndexBuff = Bind::IndexBuffer::Resolve(gfx, tag, indices);

	pTopology = Bind::Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	{
		Technique line{ "Line", Chan::main };
		Step only("Lambertian");
		auto pvs = Bind::VertexShader::Resolve(gfx, "Solid_VS.cso");
		auto pvsbc = pvs->GetBytecode();
		only.AddBindable(Bind::InputLayout::Resolve(gfx, vl, *pvs));
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
		AddTechnique(std::move(line));
	}
}

void CameraIndicator::SetPosition(DirectX::XMFLOAT3 pos) noexcept
{
	this->pos = pos;
}


void CameraIndicator::SetRotation(DirectX::XMFLOAT3 rot) noexcept
{
	this->rot = rot;
}


DirectX::XMMATRIX CameraIndicator::GetTransformXM() const noexcept 
{
	return DirectX::XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z)
		* DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
}


