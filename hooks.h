#pragma once

#include <Windows.h>
#include <dxgi.h>
#include <d3d11.h>
#include <MinHook.h>

typedef HRESULT(__stdcall* Present)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
extern Present oPresent;

void InitializeTrainer();
HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
