#pragma once
#include "Window.hpp"
#include "Timer.hpp"
#include "Box.h"
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
	Window wnd;
	Timer timer;
	std::vector<std::unique_ptr<class Box>> boxes;
};

