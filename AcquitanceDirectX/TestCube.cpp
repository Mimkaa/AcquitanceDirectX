#include "TestCube.h"
#include "Cube.h"
#include "BindableCommon.h"
#include "imgui/imgui.h"
#include "Step.h"


TestCube::TestCube(Graphics& gfx)
{
	using namespace Bind;
	namespace dx = DirectX;

	auto layout = Dvtx::VertexLayout{}.Append(Dvtx::VertexLayout::Position3D).Append(Dvtx::VertexLayout::Normal).
		Append(Dvtx::VertexLayout::Texture2D);

	auto cube = Cube::MakeIndenedentSkinned(std::move(layout));
	cube.SetNormalsIndependentFlat();
	
	std::string TagName = "cube$";
	std::string base = "images\\";


	pIndexBuff=IndexBuffer::Resolve(gfx, TagName, cube.indices);
	pVertBuff=VertexBuffer::Resolve(gfx, TagName, cube.vertices);
	pTopology=Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	
	{
		Technique single;
		{
			Step lambertine(0);
			
			lambertine.AddBindable(Texture::Resolve(gfx, base + "brickwall.jpg", 0));
			lambertine.AddBindable(Sampler::Resolve(gfx));
			auto pvs = VertexShader::Resolve(gfx, "PhongVS.cso");
			auto pvsbc = pvs->GetBytecode();

			lambertine.AddBindable(std::move(pvs));
			lambertine.AddBindable(PixelShader::Resolve(gfx, "PhongPS.cso"));
			lambertine.AddBindable(PixelConstantBuffer<PSConstant>::Resolve(gfx, Const, 1u));
			lambertine.AddBindable(InputLayout::Resolve(gfx, cube.vertices.GetLayout(), pvsbc));
			lambertine.AddBindable(std::make_shared<TransformCbuf>(gfx));

			single.AddStep(std::move(lambertine));
			

		}
		AddTechnique(std::move(single));
	}

	{
		Technique outline;
		{
			Step mask(1);

			auto pvs = VertexShader::Resolve(gfx, "SolidVS.cso");
			auto pvsbc = pvs->GetBytecode();
			mask.AddBindable(std::move(pvs));

			// TODO: better sub-layout generation tech for future consideration maybe
			mask.AddBindable(InputLayout::Resolve(gfx, cube.vertices.GetLayout(), pvsbc));

			mask.AddBindable(std::make_shared<TransformCbuf>(gfx));

			// TODO: might need to specify rasterizer when doubled-sided models start being used

			outline.AddStep(std::move(mask));
		}

		{
			Step draw(2);

			auto pvs = VertexShader::Resolve(gfx, "SolidVS.cso");
			auto pvsbc = pvs->GetBytecode();
			draw.AddBindable(std::move(pvs));

			// this can be pass-constant
			draw.AddBindable(PixelShader::Resolve(gfx, "SolidPS.cso"));

			// TODO: better sub-layout generation tech for future consideration maybe
			draw.AddBindable(InputLayout::Resolve(gfx, cube.vertices.GetLayout(), pvsbc));

			// quick and dirty... nicer solution maybe takes a lamba... we'll see :)
			class TransformCbufScaling : public TransformCbuf
			{
			public:
				using TransformCbuf::TransformCbuf;
				void Bind(Graphics& gfx) noexcept override
				{
					const auto scale = dx::XMMatrixScaling(1.04f, 1.04f, 1.04f);
					auto xf = GetTransforms(gfx);
					xf.modle = xf.modle * scale;
					xf.modelViewProj = xf.modelViewProj * scale;
					UpdateAndBind(gfx, xf);
				}
			};
			draw.AddBindable(std::make_shared<TransformCbufScaling>(gfx));

			// TODO: might need to specify rasterizer when doubled-sided models start being used

			outline.AddStep(std::move(draw));
		}
		AddTechnique(std::move(outline));
	}



}

DirectX::XMMATRIX TestCube::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(pos.pitch, pos.yaw, pos.roll)
		* DirectX::XMMatrixTranslation(pos.pos.x, pos.pos.y, pos.pos.z);

	
}

void TestCube::ShowControlWindow(Graphics& gfx) noexcept
{
	using namespace Bind;
	bool ff = static_cast<bool>(Const.normalsOn);

	if (ImGui::Begin("CubeConstrolWindow"))
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
	/*Const.normalsOn = static_cast<BOOL>(ff);
	PixelConstantBuffer<PSConstant>* pPcbuf = QueryBindables<PixelConstantBuffer<PSConstant>>();
	pPcbuf->Update(gfx, Const);
	pPcbuf->Bind(gfx);*/
	ImGui::End();


}