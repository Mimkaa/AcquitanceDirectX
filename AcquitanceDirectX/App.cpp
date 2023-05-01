#include "App.hpp"
#include <memory>
#include <algorithm>
#include "ChiliMath.h"
#include "Surface.h"
#include "ScriptCommander.h"
#include "imgui/imgui.h"
#include "VertexShader.h"
#include <shellapi.h>
#include "dxtex/DirectXTex.h"
#include"ChilliStringUtils.h"
#include "Testing.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"
#include "DynamicConstantBuffer.h"
#include "ModelProbe.h"
#include "Node.h"
#include "ChilliXM.h"
#include "Channels.h"


namespace dx = DirectX;



App::App(const std::string& commandLine )
	:
	commandLine(commandLine),
	wnd(1000, 550, L"I understood how it works)))"),
	light(wnd.Gfx()),
	sc(TokenizeQuoted(commandLine))
	
{
	
	// testing
	//TestDynamicConstantBuff();
	//TestDynamicVertexBuff();
	TestMaterialSystemLoading(wnd.Gfx());
	//plane.SetPos(cam.GetPosition());
	{
		std::string path = "Models\\brick_wall\\brick_wall.obj";
		Assimp::Importer imp;
		const auto pScene = imp.ReadFile(path,
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_ConvertToLeftHanded |
			aiProcess_GenNormals |
			aiProcess_CalcTangentSpace
		);
		Material mat{ wnd.Gfx(),*pScene->mMaterials[1],path };
		pLoaded = std::make_unique<Mesh>(wnd.Gfx(), mat, *pScene->mMeshes[0]);
	}
	
	
	/*wall.SetPos(dx::XMMatrixTranslation( 2.5f,0.0f,1.0f ));
	Gobber.SetRootTransform(dx::XMMatrixTranslation(0.0f, 0.0f, 1.0f));
	nano.SetRootTransform(dx::XMMatrixTranslation(-2.0f, -7.0f, 1.0f));*/

//wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 400.0f));
// check if we get the same pointer in this case

// Add cameras 

cams.AddCamera(std::make_unique<Camera>(wnd.Gfx(),"A", 1.2f, 0.0f, DirectX::XMFLOAT3{ 0.0f, 12.0f, 3.0f }));
cams.AddCamera(std::make_unique<Camera>(wnd.Gfx(), "B", 1.2f, 0.0f, DirectX::XMFLOAT3{ 0.0f, 15.0f, 3.0f }));
cams.AddCamera(light.GetCamera());
fc.BindShadowCamera(*light.GetCamera());
saveDepth = true;
}

App::~App()
{}

int App::Go() 
{
	while (true)
	{
		// process all messages pending, but to not block for new messages
		if (const auto ecode = Window::ProcessMessages())
		{
			
			// if return optional has value, means we're quitting so return exit code
			return *ecode;
		}
		
		DoFrame();
	}
}

void App::DoFrame()
{
	const auto dt = timer.Mark() * speed_factor;
	
	wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);
	//wnd.Gfx().SetCamera(cams.GetActiveCamera().GetMatrix());
	fc.BindMainCamera(cams.GetActiveCamera());
	light.Bind(wnd.Gfx(), cams.GetActiveCamera().GetMatrix());
	
	
	//cams.Bind(wnd.Gfx());
	
	
	sponza.Submit(fc, Chan::main);
	//Gobber.Submit(fc);
	light.Submit(fc, Chan::main);
	//cubby.Submit(fc);
	cams.Submit(fc, Chan::main);
	

	// shadow channel pass
	
	sponza.Submit(fc, Chan::shadow);
	//Gobber.Submit(fc);
	light.Submit(fc, Chan::shadow);
	//cubby.Submit(fc);
	cams.Submit(fc, Chan::shadow);
	
	fc.Execute(wnd.Gfx());
	if (saveDepth)
	{
		fc.DumpDepth(wnd.Gfx());
		//fc.SaveDepth(wnd.Gfx(),true);
		saveDepth = false;
	}
	


	class TP : public TechniqueProbe
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
			if (auto v = buf["materialColor"]; v.Exists())
			{
				dcheck(ImGui::ColorPicker3(tag("Color"), reinterpret_cast<float*>(&static_cast<dx::XMFLOAT3&>(v))));
			}
			if (auto v = buf["specularColor"]; v.Exists())
			{
				dcheck(ImGui::ColorPicker3(tag("Spec. Color"), reinterpret_cast<float*>(&static_cast<dx::XMFLOAT3&>(v))));
			}
			if (auto v = buf["specularGloss"]; v.Exists())
			{
				dcheck(ImGui::SliderFloat(tag("Glossiness"), &v, 1.0f, 100.0f, "%.1f", 1.5f));
			}
			if (auto v = buf["specularWeight"]; v.Exists())
			{
				dcheck(ImGui::SliderFloat(tag("Spec. Weight"), &v, 0.0f, 2.0f));
			}
			if (auto v = buf["useNormalMap"]; v.Exists())
			{
				dcheck(ImGui::Checkbox(tag("Normal Map Enable"), &v));
			}
			if (auto v = buf["normalMapWeight"]; v.Exists())
			{
				dcheck(ImGui::SliderFloat(tag("Normal Map Weight"), &v, 0.0f, 2.0f));
			}
			if (auto v = buf["useSpecularMap"]; v.Exists())
			{
				dcheck(ImGui::Checkbox(tag("Spec. Map Enable"), &v));
			}
			if (auto v = buf["offset"]; v.Exists())
			{
				dcheck(ImGui::SliderFloat(tag("offset"), &v, 0.0f, 1.0f, "%.3f", 2.5f));
			}
			return dirty;
		}
	};
	

	class MP : ModelProbe
	{
	public:
		void SpawnWindow(Model& model)
		{
			ImGui::Begin("Model");
			ImGui::Columns(2, nullptr, true);
			model.Accept(*this);

			ImGui::NextColumn();
			if (pSelectedNode != nullptr)
			{
				TP probe;
				pSelectedNode->Accept(probe);
			}
			if (pSelectedNode != nullptr)
			{
				bool dirty = false;
				const auto dcheck = [&dirty](bool changed) {dirty = dirty || changed; };
				auto& tf = ResolveTransform();
				ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, "Translation");
				dcheck(ImGui::SliderFloat("X", &tf.x, -60.f, 60.f));
				dcheck(ImGui::SliderFloat("Y", &tf.y, -60.f, 60.f));
				dcheck(ImGui::SliderFloat("Z", &tf.z, -60.f, 60.f));
				ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, "Orientation");
				dcheck(ImGui::SliderAngle("X-rotation", &tf.xRot, -180.0f, 180.0f));
				dcheck(ImGui::SliderAngle("Y-rotation", &tf.yRot, -180.0f, 180.0f));
				dcheck(ImGui::SliderAngle("Z-rotation", &tf.zRot, -180.0f, 180.0f));
				if (dirty)
				{
					pSelectedNode->SetAppliedTransform(
						dx::XMMatrixRotationX(tf.xRot) *
						dx::XMMatrixRotationY(tf.yRot) *
						dx::XMMatrixRotationZ(tf.zRot) *
						dx::XMMatrixTranslation(tf.x, tf.y, tf.z)
					);
				}
			}
			

			ImGui::End();

			
		}
	protected:
		bool PushNode(Node& node) override
		{
			// if there is no selected node, set selectedId to an impossible value
			const int selectedId = (pSelectedNode == nullptr) ? -1 : pSelectedNode->GetId();
			// build up flags for current node
			const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow
				| ((node.GetId() == selectedId) ? ImGuiTreeNodeFlags_Selected : 0)
				| (node.HasChildren() ? 0 : ImGuiTreeNodeFlags_Leaf);
			// render this node
			const auto expanded = ImGui::TreeNodeEx(
				(void*)(intptr_t)node.GetId(),
				node_flags, node.GetName().c_str()
			);
			// processing for selecting node
			if (ImGui::IsItemClicked())
			{
				class ProbeSelecter :public TechniqueProbe
				{
				public:
					bool OnVisitBuffer(Dcb::Buffer& buf) override
					{
						return true;
					}
					void OnSetTech() override
					{
						if (pTech->GetName() == "Outline")
						{
							pTech->SetActiveState(active);
						}
					}
					void SetState(bool state)
					{
						active = state;
					}
				private:
					bool active = false;
				} pprr;
				// recursively goes through all the nods and sets them to be 
				if (pSelectedNode != nullptr)
				{
					pprr.SetState(false);
					pSelectedNode->Accept(pprr);
					
				}
				pprr.SetState(true);
				node.Accept(pprr);
				pSelectedNode = &node;
			}
			// signal if children should also be recursed
			return expanded;
		}
		void PopNode(Node& node) override
		{
			ImGui::TreePop();
		}
	protected:
		Node* pSelectedNode = nullptr;
		struct TransformParameters
		{
			float xRot = 0.0f;
			float yRot = 0.0f;
			float zRot = 0.0f;
			float x = 0.0f;
			float y = 0.0f;
			float z = 0.0f;
		};
		std::unordered_map<int, TransformParameters> transformParams;
	private:
		TransformParameters& ResolveTransform() noexcept
		{
			const auto id = pSelectedNode->GetId();
			auto i = transformParams.find(id);
			if (i == transformParams.end())
			{
				return LoadTransform(id);
			}
			return i->second;
		}
		TransformParameters& LoadTransform(int id) noexcept
		{
			const auto& applied = pSelectedNode->GetAppliedTransform();
			DirectX::XMFLOAT4X4 storedAppied;
			DirectX::XMStoreFloat4x4(&storedAppied, applied);
			const auto angles = ExtractEulerAngles(storedAppied);
			const auto translation = ExtractTranslation(storedAppied);
			TransformParameters tp;
			tp.zRot = angles.z;
			tp.xRot = angles.x;
			tp.yRot = angles.y;
			tp.x = translation.x;
			tp.y = translation.y;
			tp.z = translation.z;
			return transformParams.insert({ id,{ tp } }).first->second;
		}
	};
	static MP modelProbe;

	modelProbe.SpawnWindow(sponza);
	//plane.Draw(wnd.Gfx());
	//plane.Draw(wnd.Gfx());

	

	while (const auto e = wnd.kbd.ReadKey())
	{
		if (e->IsPress() && e->GetCode() == VK_ESCAPE)
		{
			if (wnd.GetCursorenabled())
			{
				wnd.DisableCursor();
				wnd.mouse.EnableRaw();
			}
			else if (!wnd.GetCursorenabled())
			{
				wnd.EnableCursor();
				wnd.mouse.DisableRaw();
			}
		}
		

	}
	if (!wnd.GetCursorenabled())
	{
		// moving
		if (wnd.kbd.KeyIsPressed('W'))
		{
			cams->Translate({ 0.0f, 0.0f, dt });
		}
		if (wnd.kbd.KeyIsPressed('S'))
		{
			cams->Translate({ 0.0f, 0.0f, -dt });
		}
		if (wnd.kbd.KeyIsPressed('D'))
		{
			cams->Translate({ dt, 0.0f, 0.0f });
		}
		if (wnd.kbd.KeyIsPressed('A'))
		{
			cams->Translate({ -dt, 0.0f, 0.0f });
		}
		if (wnd.kbd.KeyIsPressed('R'))
		{
			cams->Translate({ 0.0f, dt, 0.0f });
		}
		if (wnd.kbd.KeyIsPressed('F'))
		{
			cams->Translate({ 0.0f, -dt, 0.0f });
		}
		if (wnd.kbd.KeyIsPressed('L'))
		{
			saveDepth = true;
		}

		
	}
	while (const auto d = wnd.mouse.ReadRawDelta())
	{
		if (!wnd.GetCursorenabled())
		{
			cams->Rotate(d->x, d->y);
		}

	}
	

	// control model
	//plane.ShowControlWindow(wnd.Gfx());
	//sponza.ShowWindow(wnd.Gfx(), "Sponza");
	/*Gobber.ShowWindow(wnd.Gfx(), "Gobber") ;
	nano.ShowWindow(wnd.Gfx(), "nano");
	wall.ShowControlWindow(wnd.Gfx());*/
	
	// imgui window for camera
	
	cams.ControlWindow(wnd.Gfx());
	light.SpawnControlWindow();
	cubby.ShowControlWindow(wnd.Gfx());
	
	//fc.ShowWindows(wnd.Gfx());
	

	fc.Reset();
	
	//wnd.Gfx().GetTarget()->ToSurface(wnd.Gfx()).Save("screen.png");

	
	wnd.Gfx().EndFrame();
	

		
}
