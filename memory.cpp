#include "memory.h"

void* AllocateMemory(size_t size) {
    void* address = VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (address == nullptr) {
        std::cerr << "Failed to allocate memory of size: " << size << std::endl;
    }
    else {
        std::cout << "Allocated memory at: " << address << " of size: " << size << std::endl;
    }
    return address;
}

template <typename T>
T ReadMemory(uintptr_t address) {
    T value{};
    if (ReadProcessMemory(GetCurrentProcess(), reinterpret_cast<LPCVOID>(address), &value, sizeof(T), nullptr)) {
        std::cout << "Read memory from address: " << std::hex << address << " value: " << value << std::endl;
    }
    else {
        std::cerr << "Failed to read memory from address: " << std::hex << address << std::endl;
    }
    return value;
}

template <typename T>
void WriteMemory(uintptr_t address, T value) {
    if (WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<LPVOID>(address), &value, sizeof(T), nullptr)) {
        std::cout << "Wrote memory to address: " << std::hex << address << " value: " << value << std::endl;
    }
    else {
        std::cerr << "Failed to write memory to address: " << std::hex << address << std::endl;
    }
}

bool WriteMemoryArray(uintptr_t address, const char* data, size_t size) {
    DWORD oldProtect;

    if (!VirtualProtect(reinterpret_cast<LPVOID>(address), size, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        std::cerr << "Failed to change memory protection, error: " << GetLastError() << std::endl;
        return false;
    }

    memcpy(reinterpret_cast<void*>(address), data, size);

    if (!VirtualProtect(reinterpret_cast<LPVOID>(address), size, oldProtect, &oldProtect)) {
        std::cerr << "Failed to restore memory protection, error: " << GetLastError() << std::endl;
        return false;
    }

    return true;
}


template float ReadMemory<float>(uintptr_t address);
template void WriteMemory<float>(uintptr_t address, float value);
