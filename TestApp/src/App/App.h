#pragma once

#define NOMINMAX

#include <Windows.h>
#include <iostream>
#include <dwmapi.h>

#include <unordered_map>
#include <format>
#include <filesystem>

#include <d3d11.h>
#include <wrl.h>

#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_dx11.h>

class App
{
public:
	App(const wchar_t* title, int width, int height);
	virtual ~App();

	int Run();
protected:
	void Tick();

protected:
	virtual void OnStart() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnClose() = 0;

	virtual void OnPreRender() = 0;
	virtual void OnRender() = 0;
	//virtual void OnPostRender() = 0;

	virtual void OnSizeChanged(int width, int height) = 0;

	virtual LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) = 0;
private:
	static LRESULT CALLBACK WndProcSetup(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	static LRESULT CALLBACK WndProcDispatch(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
protected:
	bool CreateAppWindow();
	void CleanupAppWindow();

	bool CreateDevice();
	void CleanupDevice();

	bool CreateRenderTarget();
	void CleanupRenderTarget();
protected:
	bool m_running;

	HWND m_window;
	WNDCLASSEX m_wc_ex;

	int m_width;
	int m_height;
	const wchar_t* m_title;

	Microsoft::WRL::ComPtr<ID3D11Device> m_device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_device_ctx;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_swap_chain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_render_target_view;
private:
	bool m_wnd_in_sizemove = false;
	bool m_wnd_minimized = false;
};

