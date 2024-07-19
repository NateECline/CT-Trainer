#include "hooks.h"
#include "kiero.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include <Windows.h>
#include <iostream>
#include <dxgi.h>
#include <d3d11.h>
#include <MinHook.h>

typedef HRESULT(__stdcall* Present)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
Present oPresent = nullptr;
HWND gameWindow = nullptr;

void InitImGui(IDXGISwapChain* pSwapChain);

void InitializeTrainer()
{
    std::cout << "Starting InitializeTrainer..." << std::endl;

    if (MH_Initialize() != MH_OK) {
        std::cerr << "Failed to initialize MinHook" << std::endl;
        return;
    }

    HMODULE hModule = GetModuleHandleA("ChainedTogether-Win64-Shipping.exe");
    if (hModule == nullptr) {
        std::cerr << "Failed to get module handle for ChainedTogether-Win64-Shipping.exe" << std::endl;
        return;
    }
    std::cout << "Successfully got module handle for ChainedTogether-Win64-Shipping.exe" << std::endl;

    kiero::Status::Enum initStatus = kiero::init(kiero::RenderType::D3D11);
    std::cout << "Kiero initialization status: " << static_cast<int>(initStatus) << std::endl;

    if (initStatus == kiero::Status::ModuleNotFoundError) {
        std::cerr << "ModuleNotFoundError: Ensure the game is using DirectX 11." << std::endl;

        HMODULE d3d11Module = GetModuleHandleA("d3d11.dll");
        if (d3d11Module == nullptr) {
            d3d11Module = LoadLibraryA("d3d11.dll");
        }
        if (d3d11Module == nullptr) {
            std::cerr << "Failed to load d3d11.dll" << std::endl;
            return;
        }
        else {
            std::cout << "Successfully loaded d3d11.dll" << std::endl;
            initStatus = kiero::init(kiero::RenderType::D3D11);
            std::cout << "Kiero re-initialization status: " << static_cast<int>(initStatus) << std::endl;
        }
    }

    if (initStatus != kiero::Status::Success) {
        if (initStatus == kiero::Status::ModuleNotFoundError)
            std::cout << "ModuleNotFoundError: Ensure the game is using DirectX 11." << std::endl;
        else if (initStatus == kiero::Status::NotSupportedError)
            std::cout << "NotSupportedError: The selected render type is not supported." << std::endl;
        else if (initStatus == kiero::Status::AlreadyInitializedError)
            std::cout << "AlreadyInitializedError: Kiero is already initialized." << std::endl;
        else
            std::cout << "Unknown error during Kiero initialization." << std::endl;
        return;
    }

    std::cout << "Kiero initialization succeeded." << std::endl;

    kiero::Status::Enum bindStatus = kiero::bind(8, (void**)&oPresent, hkPresent);
    std::cout << "Kiero bind status: " << static_cast<int>(bindStatus) << std::endl;

    if (bindStatus != kiero::Status::Success)
    {
        std::cout << "Kiero bind failed." << std::endl;
        return;
    }

    std::cout << "Kiero bind succeeded." << std::endl;
}

HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
    static bool init = false;
    if (!init)
    {
        std::cout << "Initializing ImGui..." << std::endl;
        InitImGui(pSwapChain);
        init = true;
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Trainer");
    ImGui::Text("This is a box!");
    ImGui::Button("Click me");
    ImGui::End();

    ImGui::Render();
    ID3D11DeviceContext* context = nullptr;
    pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&context);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    context->Release();

    return oPresent(pSwapChain, SyncInterval, Flags);
}

void InitImGui(IDXGISwapChain* pSwapChain)
{
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* context = nullptr;
    ID3D11RenderTargetView* rtv = nullptr;

    if (FAILED(pSwapChain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&device))))
    {
        std::cout << "Failed to get D3D11 device." << std::endl;
        return;
    }
    std::cout << "D3D11 device acquired." << std::endl;

    device->GetImmediateContext(&context);

    ID3D11Texture2D* backBuffer = nullptr;
    if (FAILED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer)))
    {
        std::cout << "Failed to get back buffer." << std::endl;
        return;
    }
    std::cout << "Back buffer acquired." << std::endl;

    if (FAILED(device->CreateRenderTargetView(backBuffer, NULL, &rtv)))
    {
        backBuffer->Release();
        std::cout << "Failed to create render target view." << std::endl;
        return;
    }
    std::cout << "Render target view created." << std::endl;
    backBuffer->Release();

    ImGui::CreateContext();
    ImGui_ImplWin32_Init(gameWindow); // Use the global HWND here
    ImGui_ImplDX11_Init(device, context);

    context->OMSetRenderTargets(1, &rtv, NULL);

    rtv->Release();
    context->Release();
    device->Release();
    std::cout << "ImGui initialized." << std::endl;
}
