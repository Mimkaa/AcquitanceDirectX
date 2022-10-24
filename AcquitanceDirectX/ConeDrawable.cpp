#include "ConeDrawable.h"
#include "BindableBase.h"
#include "GraphicsThrowHeader.h"
#include "MyCone.h"
#include "array";
ConeDrawable::ConeDrawable(Graphics& gfx, std::mt19937& rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist,
	std::uniform_real_distribution<float>& bdist,
	int tessalation)
	:
	TestObj(gfx, rng, adist, ddist, odist, rdist)

{
	namespace dx = DirectX;
	if (!IsStaticInitialized())
	{
		

		
		
		auto pvs = std::make_unique<VertexShader>(gfx, L"ColorBlendPhongVS.cso");
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<PixelShader>(gfx, L"ColorBlendPhongPS.cso"));

		


		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{"Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "Color",0,DXGI_FORMAT_R8G8B8A8_UNORM,0,24,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};
		AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));
		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else
	{
		SetIndexFromStatic();
	}
	AddBind(std::make_unique<TransformCbuf>(gfx, *this, 1u));

	struct PSMaterialConstantBuf
	{
		float specularIntensity = 0.6f;
		float specularPower = 30.0f;
		float padding[2];
	} spec_buf;
	AddBind(std::make_unique<PixelConstantBuffer<PSMaterialConstantBuf>>(gfx, spec_buf, 1u));

	struct Vertex
	{
		dx::XMFLOAT3 pos;
		dx::XMFLOAT3 n;
		std::array<char, 4> color;
		char padding;

	};

	auto model = MyCone::MakeTessalated<Vertex>(tessalation);
	for (auto& v : model.vertices)
	{
		v.color = { (char)40,(char)40,(char)255 };
	}

	// set top to a different color
	for (int i = 0; i < tessalation; i++)
	{
		model.vertices[model.indices[i * 3]].color = { (char)255,(char)40,(char)40 };
	}

	model.Transform(dx::XMMatrixScaling(1.0f, 1.0f, 0.7f));

	model.SetNormalsIndependentFlat();


	AddBind(std::make_unique<VertexBuffer>(gfx, model.vertices));
	AddIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.indices));

	// model deformation transform (per instance, not stored as bind)
	DirectX::XMStoreFloat3x3(
		&mt,
		DirectX::XMMatrixScaling(1.0f, 1.0f, bdist(rng))
	);


}



DirectX::XMMATRIX ConeDrawable::GetTransformXM() const noexcept
{
	namespace dx = DirectX;
	return TestObj::GetTransformXM() * dx::XMLoadFloat3x3(&mt);
}