#pragma once

#include "kiero.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include <Windows.h>
#include <dxgi.h>
#include <d3d11.h>
#include <MinHook.h>

typedef HRESULT(__stdcall* Present)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
extern Present oPresent;

void InitializeTrainer();
HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
void InitImGui(IDXGISwapChain* pSwapChain);
