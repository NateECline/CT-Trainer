#include <Windows.h>
#include <iostream>
#include "kiero.h"
#include "hooks.h"
#include "luahandler.h"
#include "pattern_scanner.h"
#include "memory.h"

extern lua_State* L;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);

        AllocConsole();
        freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
        freopen_s(reinterpret_cast<FILE**>(stderr), "CONOUT$", "w", stderr);
        SetConsoleTitle(L"Debug Console - My Trainer");

        std::cout << "DLL Injected!" << std::endl;

        InitializeLua();

        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)InitializeTrainer, hModule, 0, nullptr);
        break;

    case DLL_PROCESS_DETACH:
        CloseLua();
        kiero::shutdown();
        FreeConsole();
        break;
    }
    return TRUE;
}
