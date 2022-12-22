
# include "Window.hpp"
#include <iostream>
#include <sstream>
#include "App.hpp"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	try {
		App{ lpCmdLine }.Go();
	}
	catch (const ChiliException& e)
	{
		MessageBox(nullptr, StringToWstring(e.what()).c_str(), StringToWstring(e.GetType()).c_str(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e)
	{
		MessageBox(nullptr, StringToWstring(e.what()).c_str(), L"Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBox(nullptr, L"No details available", L"Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	return -1;
}


