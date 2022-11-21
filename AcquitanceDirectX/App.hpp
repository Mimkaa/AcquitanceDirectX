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
	App();
	~App();
	// message loop
	int Go();
private:
	void DoFrame();
	
private:
	ImguiManager imgui;
	Window wnd;
	Timer timer;
	float speed_factor = 1.0f;
	Camera cam;
	PointLight light;
	Model nano{ wnd.Gfx(), "Models\\nano_textured\\nanosuit.obj" };
	Model nano1{ wnd.Gfx(), "Models\\nano_textured\\nanosuit.obj" };
	TestPlane plane{wnd.Gfx()};
	TestCube cube{ wnd.Gfx() };
	int x = 0;
	int y = 0;
	
};

