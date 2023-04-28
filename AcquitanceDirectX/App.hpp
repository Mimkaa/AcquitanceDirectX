#pragma once
#include "Window.hpp"
#include "Timer.hpp"
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"
#include <set>
#include "Model.h"
#include "TestCube.h"
#include "ScriptCommander.h"
#include "NormalMapTwerker.h"
#include "Material.h"
#include "FrameCommander.h"
#include "CameraContainer.h"



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
	//Camera cam;
	CameraContainer cams;
	PointLight light;
	FrameComander fc{wnd.Gfx()};
	//Model Gobber{ wnd.Gfx(), "Models\\gobber\\GoblinX.obj", 5.0f };
	//Model nano{ wnd.Gfx(), "Models\\nano_textured\\nanosuit.obj" };
	//TestPlane wall{ wnd.Gfx()};
	Model sponza{ wnd.Gfx(), "Models\\Sponza\\sponza.obj",1.0f/20};
	//TestPlane plane{ wnd.Gfx(), {0.3f,0.3f,1.0f,0.0f} };
	std::unique_ptr<Mesh> pLoaded;
	TestCube cubby{ wnd.Gfx() };
	int x = 0;
	int y = 0;
	bool saveDepth = false;
	ScriptCommander sc;

};

