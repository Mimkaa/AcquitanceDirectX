#include "App.hpp"
#include "Box.h"
#include "Cylinder.h"
#include "Pyramid.h"
#include "SkinnedCube.h"
#include "FirstMesh.h"
#include <memory>
#include <algorithm>
#include "ChiliMath.h"
#include "Surface.h"
#include "GDIPlusManager.h"
#include "imgui/imgui.h"
namespace dx = DirectX;
GDIPlusManager gdipm;


App::App()
	:
	wnd(1000, 550, L"I understood how it works)))"),
	light(wnd.Gfx())
	
{

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

	

	nano.Draw();

	light.Draw(wnd.Gfx());

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
	nano.ShowWindow("nano") ;
	
	// imgui window for camera
	cam.SpawnControlWindow();
	light.SpawnControlWindow();

	
	

	wnd.Gfx().EndFrame();
	
		
}
