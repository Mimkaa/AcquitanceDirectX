#pragma once
#include "Window.hpp"
#include "Timer.hpp"
class App
{
public:
	App();
	// message loop
	int Go();
private:
	void DoFrame();

private:
	Window wnd;
	Timer timer;
};

