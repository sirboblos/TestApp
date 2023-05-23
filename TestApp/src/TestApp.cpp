#include "TestApp.h"

#include "Resources/fa_solid_900_ttf.h"
#include "Resources/fa_regular_400_ttf.h"
#include "Resources/fontawesome.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


LRESULT TestApp::WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
    {
        return true;
    }

    if (msg == WM_GETMINMAXINFO)
    {
        auto info = reinterpret_cast<MINMAXINFO*>(lparam);

        info->ptMinTrackSize.x = 800;
        info->ptMinTrackSize.y = 450;
    }

    return DefWindowProc(hwnd, msg, wparam, lparam);
}

void TestApp::OnStart()
{
    FILE* f;
    AllocConsole();
    freopen_s(&f, "CONOUT$", "w", stdout);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    io.IniFilename = "TestApp.ini";
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGuiApp::StyleColorsDark();

    ImGui_ImplWin32_Init(m_window);
    ImGui_ImplDX11_Init(m_device.Get(), m_device_ctx.Get());

    static ImWchar font_ranges[] = { 0x0020, 0x00FF, 0 }; //{ 0x0001, 0x1FFFF, 0 };
    static ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };

    static ImFontConfig icon_config;

    icon_config.MergeMode = true;
    icon_config.FontDataOwnedByAtlas = false;

    io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", 16.0f, nullptr, font_ranges);

    io.Fonts->AddFontFromMemoryCompressedBase85TTF(fa_solid_900_ttf_compressed_data_base85, 12.0f, &icon_config, icon_ranges);
    //io.Fonts->AddFontFromMemoryCompressedBase85TTF(fa_regular_400_ttf_compressed_data_base85, 12.0f, &icon_config, icon_ranges);

    m_consolas_font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\consola.ttf", 14.0f, nullptr, font_ranges);
}

void TestApp::OnUpdate()
{

}

void TestApp::OnClose()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void TestApp::OnPreRender()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    {

    }

    ImGui::Render();
}

void TestApp::OnRender()
{
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    const ImGuiIO& io = ImGui::GetIO();

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}

void TestApp::OnSizeChanged(int width, int height)
{
    if (m_device.Get() != nullptr)
    {
        CleanupRenderTarget();
        m_swap_chain->ResizeBuffers(0, static_cast<UINT>(width), static_cast<UINT>(height), DXGI_FORMAT_UNKNOWN, 0);
        CreateRenderTarget();
    }
}
