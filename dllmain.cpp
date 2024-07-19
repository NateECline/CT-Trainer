#include <Windows.h>
#include <iostream>
#include "kiero.h"
#include "hooks.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);

        // Allocating a console and redirecting stdout to it
        AllocConsole();
        freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout); // Using freopen_s instead of freopen
        std::cout << "DLL Injected!" << std::endl;

        // Creating a thread to initialize the trainer
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)InitializeTrainer, hModule, 0, nullptr);
        break;

    case DLL_PROCESS_DETACH:
        kiero::shutdown();
        break;
    }
    return TRUE;
}
