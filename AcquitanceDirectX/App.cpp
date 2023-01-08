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
namespace dx = DirectX;



App::App(const std::string& commandLine )
	:
	commandLine(commandLine),
	wnd(1000, 550, L"I understood how it works)))"),
	light(wnd.Gfx()),
	sc(TokenizeQuoted(commandLine))
	
{
	// testing
	TestDynamicConstantBuff();
	
	plane.SetPos(cam.GetPosition());

	
	
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

	
	sponza.Draw();
	/*Gobber.Draw();
	nano.Draw();
	wall.Draw(wnd.Gfx());*/

	light.Draw(wnd.Gfx());
	plane.Draw(wnd.Gfx());
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
	plane.ShowControlWindow(wnd.Gfx());
	sponza.ShowWindow(wnd.Gfx(), "Sponza");
	/*Gobber.ShowWindow(wnd.Gfx(), "Gobber") ;
	nano.ShowWindow(wnd.Gfx(), "nano");
	wall.ShowControlWindow(wnd.Gfx());*/
	
	// imgui window for camera
	cam.SpawnControlWindow();
	light.SpawnControlWindow();

	
	

	wnd.Gfx().EndFrame();
	
		
}
