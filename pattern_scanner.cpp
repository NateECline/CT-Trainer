#include <windows.h>
#include <Psapi.h>
#include <vector>
#include <iostream>
#include <string>

#include "pattern_scanner.h"

std::vector<int> PatternToByte(const char* pattern) {
    std::vector<int> bytes;
    while (*pattern) {
        if (*pattern == ' ') {
            pattern++;
            continue;
        }
        if (*pattern == '?') {
            bytes.push_back(-1);
            if (*(pattern + 1) == '?') pattern++;
        }
        else {
            char byteStr[3] = { 0 }; 
            byteStr[0] = *pattern;
            byteStr[1] = *(pattern + 1);
            bytes.push_back(std::strtoul(byteStr, nullptr, 16));
            pattern++; 
        }
        pattern++; 
    }
    return bytes;
}

uintptr_t PatternScan(const char* module, const char* pattern) {
    MODULEINFO moduleInfo = {0};
    HMODULE hModule = GetModuleHandleA(module);
    if (!hModule) {
        std::cerr << "Failed to get module handle for: " << module << std::endl;
        return 0;
    }

    if (!GetModuleInformation(GetCurrentProcess(), hModule, &moduleInfo, sizeof(MODULEINFO))) {
        std::cerr << "Failed to get module information for: " << module << std::endl;
        return 0;
    }

    uintptr_t base = reinterpret_cast<uintptr_t>(moduleInfo.lpBaseOfDll);
    uintptr_t size = static_cast<uintptr_t>(moduleInfo.SizeOfImage);

    std::vector<int> patternBytes = PatternToByte(pattern);
    const unsigned char* scanBytes = reinterpret_cast<const unsigned char*>(base);

    size_t patternSize = patternBytes.size();
    int* patternData = patternBytes.data();

    std::cout << "Scanning module: " << module << " with base address: " << std::hex << base << " and size: " << size << std::endl;
    std::cout << "Pattern: " << pattern << std::endl;

    for (size_t i = 0; i < size - patternSize; ++i) {
        bool found = true;
        for (size_t j = 0; j < patternSize; ++j) {
            if (patternData[j] != -1 && scanBytes[i + j] != patternData[j]) {
                found = false;
                break;
            }
        }
        if (found) {
            std::cout << "Pattern found at offset: " << std::hex << i << " (Address: " << (base + i) << ")" << std::endl;
            return base + i;
        }
    }

    std::cerr << "Pattern not found in module: " << module << std::endl;
    return 0;
}
