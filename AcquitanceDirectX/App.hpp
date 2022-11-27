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
	
	Model wall{ wnd.Gfx(), "Models\\brick_wall\\brick_wall.obj" };
	
	int x = 0;
	int y = 0;
	
};

