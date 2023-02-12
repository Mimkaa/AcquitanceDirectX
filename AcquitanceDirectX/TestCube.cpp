#include "TestCube.h"
#include "Cube.h"
#include "BindableCommon.h"
#include "imgui/imgui.h"
#include "Step.h"
#include "DynamicConstantBuffer.h"
#include "TechniqueProbe.h"
#include <string>


TestCube::TestCube(Graphics& gfx)
{
	using namespace Dcb;
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
		using namespace std::string_literals;
		Technique single("shade"s);
		{
			Step lambertine(0);
			
			lambertine.AddBindable(Texture::Resolve(gfx, base + "brickwall.jpg", 0));
			lambertine.AddBindable(Sampler::Resolve(gfx));
			auto pvs = VertexShader::Resolve(gfx, "PhongVS.cso");
			auto pvsbc = pvs->GetBytecode();

			lambertine.AddBindable(std::move(pvs));
			lambertine.AddBindable(PixelShader::Resolve(gfx, "PhongPS.cso"));

			RawLayout row{};
			row.Add<Type::Float>("specularIntensity");
			row.Add<Type::Float>("specularPower");
			row.Add<Type::Bool>("normalsOn");

			auto buf = Dcb::Buffer(std::move(row));

			buf["specularIntensity"] = 1.0f;
			buf["specularPower"] = 20.0f;
			buf["normalsOn"] = true;

			lambertine.AddBindable(std::make_shared<CachingPixelConstantBufferEx>(gfx, buf, 1));
			//lambertine.AddBindable(PixelConstantBuffer<PSConstant>::Resolve(gfx, Const, 1u));
			lambertine.AddBindable(InputLayout::Resolve(gfx, cube.vertices.GetLayout(), pvsbc));
			lambertine.AddBindable(std::make_shared<TransformCbuf>(gfx));

			single.AddStep(std::move(lambertine));
			

		}
		AddTechnique(std::move(single));
	}

	{
		Technique outline("outline");
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
				TransformCbufScaling(Graphics& gfx, float scale = 1.04f)
					:
					TransformCbuf(gfx),
					buf(MakeLayout())
				{
					buf["scale"] = scale;
				}

				void Accept(TechniqueProbe& probe) override
				{
					probe.VisitBuffer(buf);
				}

				void Bind(Graphics& gfx) noexcept override
				{
					const auto scaleScal = buf["scale"];
					const auto scale = dx::XMMatrixScaling(scaleScal, scaleScal, scaleScal);
					auto xf = GetTransforms(gfx);
					xf.modle = xf.modle * scale;
					xf.modelViewProj = xf.modelViewProj * scale;
					UpdateAndBind(gfx, xf);
				}
			private:
				static Dcb::RawLayout MakeLayout()
				{
					Dcb::RawLayout lay{};
					lay.Add<Dcb::Type::Float>("scale");
					return lay;
				}
			private:
				Dcb::Buffer buf;
			};

			Dcb::RawLayout lay;
			lay.Add<Dcb::Float4>("color");
			auto buf = Dcb::Buffer(std::move(lay));
			buf["color"] = DirectX::XMFLOAT4{ 1.0f,0.4f,0.4f,1.0f };
			draw.AddBindable(std::make_shared<Bind::CachingPixelConstantBufferEx>(gfx, buf, 1u));

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

		class Probe : public TechniqueProbe
		{
		public:
			void OnSetTech() override
			{
				using namespace std::string_literals;
				ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, pTech->GetName().c_str());
				bool active = pTech->IsActive();
				ImGui::Checkbox(("Tech Active##"s + std::to_string(techIdx)).c_str(), &active);
				pTech->SetActiveState(active);
			}
			bool OnVisitBuffer(Dcb::Buffer& buf) override
			{
				namespace dx = DirectX;
				float dirty = false;
				const auto dcheck = [&dirty](bool changed) {dirty = dirty || changed; };
				auto tag = [tagScratch = std::string{}, tagString = "##" + std::to_string(bufIdx)]
				(const char* label) mutable
				{
					tagScratch = label + tagString;
					return tagScratch.c_str();
				};

				if (auto v = buf["scale"]; v.Exists())
				{
					dcheck(ImGui::SliderFloat(tag("Scale"), &v, 1.0f, 2.0f, "%.3f", 3.5f));
				}
				if (auto v = buf["color"]; v.Exists())
				{
					dcheck(ImGui::ColorPicker4(tag("Color"), reinterpret_cast<float*>(&static_cast<dx::XMFLOAT4&>(v))));
				}
				if (auto v = buf["specularIntensity"]; v.Exists())
				{
					dcheck(ImGui::SliderFloat(tag("Spec. Intens."), &v, 0.0f, 1.0f));
				}
				if (auto v = buf["specularPower"]; v.Exists())
				{
					dcheck(ImGui::SliderFloat(tag("Glossiness"), &v, 1.0f, 100.0f, "%.1f", 1.5f));
				}
				return dirty;
			}
		} probe;

		
		Accept(probe);
	}
	
	ImGui::End();


}