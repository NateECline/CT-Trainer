#include "gui.h"
#include "imgui.h"
#include "backends/imgui_impl_dx11.h"
#include "backends/imgui_impl_win32.h"
#include "luahandler.h"
#include "memory.h"
#include "pattern_scanner.h"
#include "lua_headers.h"
#include <iostream>

static HWND gameWindow = nullptr;
static ID3D11RenderTargetView* rtv = nullptr;
static ID3D11Device* device = nullptr;
static ID3D11DeviceContext* context = nullptr;
static WNDPROC oWndProc = nullptr;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void InitImGui(IDXGISwapChain* pSwapChain) {
    DXGI_SWAP_CHAIN_DESC sd;
    HRESULT result = pSwapChain->GetDesc(&sd);
    gameWindow = sd.OutputWindow;

    result = pSwapChain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&device));
    device->GetImmediateContext(&context);

    ID3D11Texture2D* backBuffer = nullptr;
    result = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
    device->CreateRenderTargetView(backBuffer, NULL, &rtv);
    backBuffer->Release();

    ImGui::CreateContext();
    ImGui_ImplWin32_Init(gameWindow);
    ImGui_ImplDX11_Init(device, context);

    oWndProc = (WNDPROC)SetWindowLongPtr(gameWindow, GWLP_WNDPROC, (LONG_PTR)WndProc);

    std::cout << "ImGui initialized with window: " << gameWindow << std::endl;
}

extern lua_State* L;

void ExecuteLuaScriptWithDebug(const char* script) {
    std::cout << "Executing Lua script with debug..." << std::endl;
    if (luaL_dostring(L, script) != LUA_OK) {
        const char* error = lua_tostring(L, -1);
        std::cerr << "Lua error: " << error << std::endl;
        lua_pop(L, 1);
    }
    else {
        std::cout << "Lua script executed successfully." << std::endl;
    }
}

void RenderImGui() {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Trainer");

    if (ImGui::Button("Execute Hello World Script")) {
        ExecuteLuaScriptWithDebug(hello_world_script);
    }

    if (ImGui::Button("Execute Coordinates Script")) {
        std::cout << "Coordinates script button clicked." << std::endl;
        ExecuteLuaScriptWithDebug(coordinates_script);
    }

    static char luaCode[256] = "print('Hello from Lua Code!')";
    ImGui::InputText("Lua Code", luaCode, IM_ARRAYSIZE(luaCode));
    if (ImGui::Button("Execute Lua Code")) {
        std::cout << "Executing Lua Code from ImGui input." << std::endl;
        ExecuteLuaScriptWithDebug(luaCode);
    }

    static float x = 0, y = 0, z = 0;
    const char* clipboardText = ImGui::GetClipboardText();
    if (clipboardText != nullptr) {
        sscanf_s(clipboardText, "Coordinates - X: %f, Y: %f, Z: %f", &x, &y, &z);
        ImGui::Text("Coordinates - X: %.6f Y: %.6f Z: %.6f", x, y, z);
    }

    static char aobPattern[256] = "";
    ImGui::InputText("AOB Pattern", aobPattern, IM_ARRAYSIZE(aobPattern));
    if (ImGui::Button("Scan for AOB")) {
        uintptr_t foundAddress = PatternScan("ChainedTogether-Win64-Shipping.exe", aobPattern);
        if (foundAddress != 0) {
            ImGui::Text("Pattern found at: %p", (void*)foundAddress);
        }
        else {
            ImGui::Text("Pattern not found.");
        }
    }

    ImGui::End();
    ImGui::Render();
    context->OMSetRenderTargets(1, &rtv, nullptr);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
        return true;

    switch (uMsg) {
    case WM_KEYDOWN:
        if (wParam == VK_F4) {
            // Toggle ImGui visibility
        }
        break;
    }

    auto& io = ImGui::GetIO();
    if (io.WantCaptureMouse || io.WantCaptureKeyboard)
        return true;

    return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

void CleanupRenderTarget() {
    if (rtv) {
        rtv->Release();
        rtv = nullptr;
    }
}

void CreateRenderTarget(IDXGISwapChain* pSwapChain) {
    ID3D11Texture2D* backBuffer = nullptr;
    if (FAILED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer))) {
        std::cout << "Failed to get back buffer." << std::endl;
        return;
    }

    ID3D11Device* device = nullptr;
    pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&device);

    if (FAILED(device->CreateRenderTargetView(backBuffer, NULL, &rtv))) {
        backBuffer->Release();
        std::cout << "Failed to create render target view." << std::endl;
        return;
    }
    backBuffer->Release();
    device->Release();
}