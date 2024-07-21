#pragma once
#include <Windows.h>
#include <dxgi.h>
#include <d3d11.h>

void InitImGui(IDXGISwapChain* pSwapChain);
void RenderImGui();
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void CleanupRenderTarget();
void CreateRenderTarget(IDXGISwapChain* pSwapChain);
