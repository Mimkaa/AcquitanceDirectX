#pragma once
#include "Window.hpp"
#include "Timer.hpp"
#include "Box.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"
#include <set>
#include "Mesh.h"
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
	Model nano{ wnd.Gfx(), "Models\\nano_hierarchy.gltf" };
	int x = 0;
	int y = 0;
	
};

