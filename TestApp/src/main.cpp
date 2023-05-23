#include <Windows.h>
#include <memory>

#include "TestApp.h"

BOOL APIENTRY WinMain(HINSTANCE instance, HINSTANCE prev_instance, PSTR cmd_line, INT cmd_show)
{
	auto app = std::make_unique<TestApp>(L"TestApp", 1280, 720);

	return app->Run();
}