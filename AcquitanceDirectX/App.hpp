#pragma once
#include "Window.hpp"
#include "Timer.hpp"
#include "Box.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"
#include <set>
#include "Model.h"
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
	Model Kakashi{ wnd.Gfx(), "Models\\nano.obj" };
};

