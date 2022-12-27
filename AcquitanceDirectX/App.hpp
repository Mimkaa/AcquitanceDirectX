#pragma once
#include "Window.hpp"
#include "Timer.hpp"
#include "Box.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"
#include <set>
#include "Mesh.h"
#include "TestPlane.h"
#include "TestCube.h"

class App
{
public:
	App(const std::string& commandLine = "");
	~App();
	// message loop
	int Go();
private:
	void DoFrame();
	
private:
	std::string commandLine;
	ImguiManager imgui;
	Window wnd; 
	Timer timer;
	float speed_factor = 1.0f;
	Camera cam;
	PointLight light;
	
	//Model Gobber{ wnd.Gfx(), "Models\\gobber\\GoblinX.obj" };
	//Model nano{ wnd.Gfx(), "Models\\nano_textured\\nanosuit.obj" };
	//TestPlane wall{ wnd.Gfx()};
	Model sponza{ wnd.Gfx(), "Models\\Sponza\\sponza.obj",1.0f/20};
	TestPlane plane{ wnd.Gfx(), {0.3f,0.3f,1.0f,0.0f} };
	int x = 0;
	int y = 0;
	
};

