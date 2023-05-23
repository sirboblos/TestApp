#include "App.h"

App::App(const wchar_t* title, int width, int height) :
    m_running(true),
    m_window(nullptr),
    m_title(title),
    m_width(width),
    m_height(height),
    m_wc_ex({})
{

}

App::~App()
{

}

int App::Run()
{
    CreateAppWindow();
    CreateDevice();

	OnStart();

    while (m_running)
    {
        MSG msg = { };

        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT)
            {
                m_running = false;
            }
        }

        if (!m_running)
        {
            break;
        }
   
        Tick();
    }

    OnClose();

    CleanupDevice();
    CleanupAppWindow();

    return 0;
}

void App::Tick()
{
    OnUpdate();

    OnPreRender();

    const float clear_color[4] = { 0.00f, 0.00f, 0.00f, 1.00f };

    m_device_ctx->OMSetRenderTargets(1, m_render_target_view.GetAddressOf(), nullptr);
    m_device_ctx->ClearRenderTargetView(m_render_target_view.Get(), clear_color);

    OnRender();

    m_swap_chain->Present(1, 0);

    //OnPostRender();
}

LRESULT App::WndProcSetup(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    if (msg == WM_NCCREATE)
    {
        CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);

        App* const app = reinterpret_cast<App*>(create_struct->lpCreateParams);

        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(app));
        SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(App::WndProcDispatch));

        return app->WndProc(hwnd, msg, wparam, lparam);
    }

    return DefWindowProc(hwnd, msg, wparam, lparam);
}

LRESULT App::WndProcDispatch(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    App* const app = reinterpret_cast<App*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

    switch (msg)
    {
    case WM_ENTERSIZEMOVE:
    {
        app->m_wnd_in_sizemove = true;
        break;
    }
    case WM_EXITSIZEMOVE:
    {
        app->m_wnd_in_sizemove = false;
        break;
    }
    case WM_SIZE:
    {
        if (wparam == SIZE_MINIMIZED)
        {
            if (!app->m_wnd_minimized)
            {
                app->m_wnd_minimized = true;
            }
        }
        else if (app->m_wnd_minimized)
        {
            app->m_wnd_minimized = false;
        }
        else if (app->m_wnd_in_sizemove)
        {
            app->OnSizeChanged(LOWORD(lparam), HIWORD(lparam));
            app->Tick();
        }
        else
        {
            app->OnSizeChanged(LOWORD(lparam), HIWORD(lparam));
        }

        break;
    }
    case WM_SYSCOMMAND:
    {
        if ((wparam & 0xFFF0) == SC_KEYMENU)
        {
            return 0;
        }

        break;
    }
    case WM_DESTROY:
    {
        PostQuitMessage(0);

        return 0;
    }
    }

    return app->WndProc(hwnd, msg, wparam, lparam);
}

bool App::CreateAppWindow()
{
    m_wc_ex = 
    { 
        sizeof(m_wc_ex),
        CS_VREDRAW | CS_HREDRAW,
        WndProcSetup,
        0, 0,
        GetModuleHandle(nullptr),
        LoadIcon(nullptr, IDI_APPLICATION),
        LoadCursor(nullptr, IDC_ARROW),
        reinterpret_cast<HBRUSH>(COLOR_BACKGROUND + 1),
        nullptr,
        L"FuckYouWindowClass1234",
        nullptr
    };

    if (!RegisterClassEx(&m_wc_ex))
    {
        MessageBox(nullptr, L"Couldn't create the window class", L"Error", MB_OK | MB_ICONERROR);

        return false;
    }

    m_window = CreateWindowEx(0, m_wc_ex.lpszClassName, m_title, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, m_width, m_height, nullptr, nullptr, m_wc_ex.hInstance, this);

    if (m_window == nullptr)
    {
        MessageBox(nullptr, L"Couldn't create the window", L"Error", MB_OK | MB_ICONERROR);
        CleanupAppWindow();

        return false;
    }

    COLORREF caption_color = RGB(36, 36, 36);
    COLORREF border_color = RGB(48, 48, 48);

    DWM_WINDOW_CORNER_PREFERENCE corner_pref = DWMWCP_ROUNDSMALL;

    DwmSetWindowAttribute(m_window, DWMWA_CAPTION_COLOR, &caption_color, sizeof(caption_color));
    DwmSetWindowAttribute(m_window, DWMWA_BORDER_COLOR, &border_color, sizeof(border_color));


    DwmSetWindowAttribute(m_window, DWMWA_WINDOW_CORNER_PREFERENCE, &corner_pref, sizeof(corner_pref));

    ShowWindow(m_window, SW_SHOWDEFAULT);
    UpdateWindow(m_window);

    return true;
}

void App::CleanupAppWindow()
{
    if (m_window != nullptr)
    {
        DestroyWindow(m_window);
    }

    UnregisterClass(m_wc_ex.lpszClassName, m_wc_ex.hInstance);
}

bool App::CreateDevice()
{
    HRESULT hr = S_OK;

    DXGI_SWAP_CHAIN_DESC sd = { };

    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = m_window;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT device_flags = 0;

    D3D_FEATURE_LEVEL feature_level;
    const D3D_FEATURE_LEVEL feature_levels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
    };

    hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        device_flags,
        feature_levels,
        ARRAYSIZE(feature_levels),
        D3D11_SDK_VERSION,
        &sd,
        &m_swap_chain,
        &m_device,
        &feature_level,
        &m_device_ctx
    );
    
    if (FAILED(hr))
    {
        return false;
    }

    return CreateRenderTarget();
}

void App::CleanupDevice()
{

}

bool App::CreateRenderTarget()
{
    HRESULT hr = S_OK;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> back_buffer;

    hr = m_swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));

    if (FAILED(hr))
    {
        return false;
    }

    hr = m_device->CreateRenderTargetView(back_buffer.Get(), nullptr, &m_render_target_view);

    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

void App::CleanupRenderTarget()
{
    m_render_target_view.Reset();
}
