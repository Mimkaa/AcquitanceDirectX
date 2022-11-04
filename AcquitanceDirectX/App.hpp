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
	void ControlModelPos();
private:
	ImguiManager imgui;
	Window wnd;
	Timer timer;
	float speed_factor = 1.0f;
	Camera cam;
	PointLight light;
	Model nano{ wnd.Gfx(), "Models\\nanosuit.obj" };
	struct {
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
		float roll = 0.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;
	}ModelPos;
};

