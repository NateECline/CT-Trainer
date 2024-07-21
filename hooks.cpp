#include "hooks.h"
#include "kiero.h"
#include "gui.h"
#include <iostream>
#include <assert.h>

Present oPresent = nullptr;

void InitializeTrainer() {
    std::cout << "Starting InitializeTrainer..." << std::endl;
    if (MH_Initialize() != MH_OK) {
        std::cerr << "Failed to initialize MinHook." << std::endl;
        return;
    }
    HMODULE hModule = GetModuleHandleA("ChainedTogether-Win64-Shipping.exe");
    if (!hModule) {
        std::cerr << "Failed to get module handle." << std::endl;
        return;
    }
    std::cout << "Successfully got module handle." << std::endl;

    if (kiero::init(kiero::RenderType::D3D11) != kiero::Status::Success) {
        std::cerr << "Failed to initialize Kiero." << std::endl;
        return;
    }
    std::cout << "Kiero initialized successfully." << std::endl;

    if (kiero::bind(8, (void**)&oPresent, hkPresent) != kiero::Status::Success) {
        std::cerr << "Failed to bind Kiero." << std::endl;
        return;
    }
    std::cout << "Kiero bind succeeded." << std::endl;
}

HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
    static bool init = false;
    if (!init) {
        std::cout << "Initializing ImGui..." << std::endl;
        InitImGui(pSwapChain);
        init = true;
    }

    RenderImGui();

    return oPresent(pSwapChain, SyncInterval, Flags);
}
