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

wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 400.0f));
// check if we get the same pointer in this case


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

	light.Bind(wnd.Gfx(), wnd.Gfx().GetCamera());
	
	wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);
	wnd.Gfx().SetCamera(cam.GetMatrix());

	
	//sponza.Draw();
	/*Gobber.Draw();
	nano.Draw();
	wall.Draw(wnd.Gfx());*/

	light.Submit(fc);
	cubby.Submit(fc);
	


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
			return dirty;
		}
	} probe;
	pLoaded->Accept(probe);
	pLoaded->Submit(fc, DirectX::XMMatrixIdentity()* DirectX::XMMatrixTranslation(0u, 13u, 5u));

	//plane.Draw(wnd.Gfx());
	//plane.Draw(wnd.Gfx());

	fc.Execute(wnd.Gfx());

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
			cam.Translate({ 0.0f, 0.0f, dt });
		}
		if (wnd.kbd.KeyIsPressed('S'))
		{
			cam.Translate({ 0.0f, 0.0f, -dt });
		}
		if (wnd.kbd.KeyIsPressed('D'))
		{
			cam.Translate({ dt, 0.0f, 0.0f });
		}
		if (wnd.kbd.KeyIsPressed('A'))
		{
			cam.Translate({ -dt, 0.0f, 0.0f });
		}
		if (wnd.kbd.KeyIsPressed('R'))
		{
			cam.Translate({ 0.0f, dt, 0.0f });
		}
		if (wnd.kbd.KeyIsPressed('F'))
		{
			cam.Translate({ 0.0f, -dt, 0.0f });
		}

		
	}
	while (const auto d = wnd.mouse.ReadRawDelta())
	{
		if (!wnd.GetCursorenabled())
		{
			cam.Rotate(d->x, d->y);
		}

	}
	

	// control model
	//plane.ShowControlWindow(wnd.Gfx());
	//sponza.ShowWindow(wnd.Gfx(), "Sponza");
	/*Gobber.ShowWindow(wnd.Gfx(), "Gobber") ;
	nano.ShowWindow(wnd.Gfx(), "nano");
	wall.ShowControlWindow(wnd.Gfx());*/
	
	// imgui window for camera
	cam.SpawnControlWindow();
	light.SpawnControlWindow();
	cubby.ShowControlWindow(wnd.Gfx());
	
	
	fc.Reset();

	wnd.Gfx().EndFrame();
	
		
}
