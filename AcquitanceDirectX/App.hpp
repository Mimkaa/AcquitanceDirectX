#pragma once
#include "Window.hpp"
#include "Timer.hpp"
#include "Box.h"
#include "ImguiManager.h"
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
	std::vector<std::unique_ptr<class Drawable>> drawables;
	float speed_factor = 1.0f;
	static constexpr size_t nDrawables = 180;
};

