#pragma once

#define NOMINMAX

#include <Windows.h>
#include <ShObjIdl.h>
#include <ShObjIdl_core.h>

#include <vector>
#include <fstream>
#include <random>
#include <memory>

#include <imgui/imgui_internal.h>
#include <imgui/imgui_stdlib.h>
#include <imgui/imgui_freetype.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_dx11.h>

#include <imgui/imgui_style.h>

#include "App/App.h"

class TestApp : public App
{
public:
	using App::App;
private:
	LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) override;

	void OnStart() override;
	void OnUpdate() override;
	void OnClose() override;

	void OnPreRender() override;
	void OnRender() override;
	//void OnPostRender() override;

	void OnSizeChanged(int width, int height) override;
public:
	ImFont* m_consolas_font;
};