#include "App.hpp"
#include "Melon.h"
#include "Pyramid.h"
#include "Box.h"
#include "Cylinder.h"
#include <memory>
#include <algorithm>
#include "ChiliMath.h"
#include "GDIPlusManager.h"
#include "Surface.h"
#include "Sheet.h"
#include "SkinnedCube.h"
#include "imgui/imgui.h"
#include "ConeDrawable.h"
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include "FirstMesh.h"
#include "VertexLayout.h"
namespace dx = DirectX;
GDIPlusManager gdipm;

void f()
{
	VertexLayout vl;
	vl.Append<VertexLayout::Position3D>().Append<VertexLayout::Normal>();
	VertexBuffer vb(vl);
	vb.EmplaceBack(dx::XMFLOAT3{ 1.0f,1.5f, 1.0f }, dx::XMFLOAT3{ 1.0f,1.5f, 1.0f });
	dx::XMFLOAT3 n = vb[0].Attr<VertexLayout::Normal>();
}
App::App()
	:
	wnd(800, 600, L"I understood how it works)))"),
	light(wnd.Gfx())
{
	f();
	class Factory
	{
	public:
		Factory(Graphics & gfx)
			:
			gfx(gfx)
		{}
		std::unique_ptr<Drawable> operator()()
		{
			switch (typedist(rng)) {
			case (0):
			{
				const dx::XMFLOAT3 mat_color = { colordist(rng), colordist(rng) ,colordist(rng) };
				return std::make_unique<Box>(
					gfx, rng, adist, ddist,
					odist, rdist, bdist, mat_color
					);
				break;
			}

			case(1):
			{
				return std::make_unique<Cylinder>(
					gfx, rng, adist, ddist,
					odist, rdist, bdist, longdist);
				break;
			}

			case(2):
			{
				int tes = tessdist(rng);
				return std::make_unique<ConeDrawable>(
					gfx, rng, adist, ddist,
					odist, rdist, bdist, tes);
				break;
			}
			case(3):
			{
				return std::make_unique<SkinnedCube>(
					gfx, rng, adist, ddist,
					odist, rdist);
				break;
			}
			case(4):
			{
				const dx::XMFLOAT3 mat_color = { colordist(rng), colordist(rng) ,colordist(rng) };
				return std::make_unique<FirstMesh>(
					gfx, rng, adist, ddist,
					odist, rdist, mat_color, 0.7);
				break;
			}

			}
		}
	private:
		Graphics& gfx;
		std::mt19937 rng{ std::random_device{}() };
		std::uniform_real_distribution<float> adist{ 0.0f,PI * 2.0f };
		std::uniform_real_distribution<float> ddist{ 0.0f,PI * 0.5f };
		std::uniform_real_distribution<float> odist{ 0.0f,PI * 0.08f };
		std::uniform_real_distribution<float> rdist{ 6.0f,20.0f };
		std::uniform_real_distribution<float> bdist{ 0.4f,3.0f };
		std::uniform_int_distribution<int> latdist{ 5,20 };
		std::uniform_int_distribution<int> longdist{ 10,40 };
		std::uniform_int_distribution<int> tessdist{ 3,30 };
		std::uniform_int_distribution<int> typedist{ 0,4};
		std::uniform_real_distribution<float> colordist{ 0.0f,1.0f };
};

Factory f(wnd.Gfx());
drawables.reserve(nDrawables);
std::generate_n(std::back_inserter(drawables), nDrawables, f);

// init our box pointers
for (auto& d : drawables)
{
	if (auto ptr = dynamic_cast<Box*>(d.get()))
	{
		boxes.push_back(ptr);
	}
}


wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));

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

	for (auto& b : drawables)
	{
		b->Update(wnd.kbd.KeyIsPressed(VK_SPACE)? 0.0f:dt);
		b->Draw(wnd.Gfx());
	}

	light.Draw(wnd.Gfx());


	// imgui window to control simulation speed
	SpawnSimulationWindow();
	
	// imgui window for camera
	cam.SpawnControlWindow();
	light.SpawnControlWindow();

	//boxes control
	SpawnBoxWindowManagerWindow();
	SpawnBoxWindows();

	wnd.Gfx().EndFrame();
	
		
}

void App::SpawnSimulationWindow()noexcept
{
	// imgui window to control simulation speed
	if (ImGui::Begin("Simulation Speed"))
	{
		ImGui::SliderFloat("Speed Factor", &speed_factor, 0.0f, 4.0f);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	}
	ImGui::End();
}

void App::SpawnBoxWindows()noexcept
{
	// imgui box attribute control windows
	for (auto i = boxControlIds.begin(); i != boxControlIds.end();)
	{
		if (!boxes[*i]->SpawnControlWindow(*i, wnd.Gfx()))
		{
			i = boxControlIds.erase(i);
		}
		else
		{
			i++;
		}
	}
	
}

void App::SpawnBoxWindowManagerWindow()noexcept
{
	// boxes control window
	// imgui window to open box windows
	if (ImGui::Begin("Boxes"))
	{
		using namespace std::string_literals;
		const auto preview = comboBoxIndex ? std::to_string(*comboBoxIndex) : "Choose a box..."s;
		if (ImGui::BeginCombo("Box Number", preview.c_str()))
		{
			for (int i = 0; i < boxes.size(); i++)
			{
				const bool selected = comboBoxIndex ? *comboBoxIndex == i : false;
				if (ImGui::Selectable(std::to_string(i).c_str(), selected))
				{
					comboBoxIndex = i;
				}
				if (selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		if (ImGui::Button("Spawn Control Window") && comboBoxIndex)
		{
			boxControlIds.insert(*comboBoxIndex);
			comboBoxIndex.reset();
		}
	}
	ImGui::End();
}