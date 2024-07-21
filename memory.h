#ifndef MEMORY_H
#define MEMORY_H

#include <cstdint>
#include <iostream>
#include <windows.h>

void* AllocateMemory(size_t size);

template <typename T>
T ReadMemory(uintptr_t address);

template <typename T>
void WriteMemory(uintptr_t address, T value);

bool WriteMemoryArray(uintptr_t address, const char* data, size_t size);

#endif
