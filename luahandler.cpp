#include "luahandler.h"
#include "pattern_scanner.h"
#include "memory.h"
#include <iostream>

lua_State* L;

void RedirectLuaPrint() {
    lua_pushcfunction(L, [](lua_State* L) -> int {
        int nargs = lua_gettop(L);
        for (int i = 1; i <= nargs; i++) {
            if (lua_isstring(L, i)) {
                std::cout << lua_tostring(L, i);
            }
            else {
                std::cout << lua_typename(L, lua_type(L, i));
            }
            if (i != nargs) std::cout << "\t";
        }
        std::cout << std::endl;
        return 0;
        });
    lua_setglobal(L, "print");
}

void InitializeLua() {
    L = luaL_newstate();
    luaL_openlibs(L);
    std::cout << "Lua initialized." << std::endl;
    RedirectLuaPrint(); 
    RegisterLuaFunctions(L); 
}

void ExecuteLuaScript(const char* script) {
    std::cout << "Executing Lua script..." << std::endl;
    if (luaL_loadstring(L, script) != LUA_OK) {
        std::cerr << "Lua error loading script: " << lua_tostring(L, -1) << std::endl;
        lua_pop(L, 1);
        return;
    }
    if (lua_pcall(L, 0, LUA_MULTRET, 0) != LUA_OK) {
        std::cerr << "Lua error executing script: " << lua_tostring(L, -1) << std::endl;
        lua_pop(L, 1);
    }
    else {
        std::cout << "Executed Lua script." << std::endl;
    }
}

void ExecuteLuaString(const char* luaCode) {
    std::cout << "Executing Lua code string..." << std::endl;
    if (luaL_loadstring(L, luaCode) != LUA_OK) {
        std::cerr << "Lua error loading string: " << lua_tostring(L, -1) << std::endl;
        lua_pop(L, 1);
        return;
    }
    if (lua_pcall(L, 0, LUA_MULTRET, 0) != LUA_OK) {
        std::cerr << "Lua error executing string: " << lua_tostring(L, -1) << std::endl;
        lua_pop(L, 1);
    }
    else {
        std::cout << "Executed Lua code string." << std::endl;
    }
}

void CloseLua() {
    if (L) {
        lua_close(L);
        L = nullptr;
        std::cout << "Lua closed." << std::endl;
    }
}

int LuaReadMemory(lua_State* L) {
    uintptr_t address = (uintptr_t)lua_tonumber(L, 1);
    float value = ReadMemory<float>(address);
    lua_pushnumber(L, value);
    return 1;
}

int LuaWriteMemory(lua_State* L) {
    uintptr_t address = (uintptr_t)lua_tonumber(L, 1);
    float value = (float)lua_tonumber(L, 2);
    WriteMemory<float>(address, value);
    return 0;
}

int LuaWriteMemoryArray(lua_State* L) {
    uintptr_t address = static_cast<uintptr_t>(lua_tonumber(L, 1));
    size_t length = static_cast<size_t>(lua_tonumber(L, 3));
    const char* data = lua_tostring(L, 2);

    bool success = WriteMemoryArray(address, data, length);
    lua_pushboolean(L, success);
    return 1;
}

int LuaPatternScan(lua_State* L) {
    const char* module = lua_tostring(L, 1);
    const char* pattern = lua_tostring(L, 2);
    uintptr_t address = PatternScan(module, pattern);
    if (address != 0) {
        lua_pushnumber(L, (lua_Number)address);
        return 1;
    }
    else {
        lua_pushnil(L);
        return 1;
    }
}

int LuaAllocateMemory(lua_State* L) {
    size_t size = (size_t)lua_tonumber(L, 1);
    void* address = AllocateMemory(size);
    lua_pushlightuserdata(L, address);
    return 1;
}

void RegisterLuaFunctions(lua_State* L) {
    lua_register(L, "ReadMemory", LuaReadMemory);
    lua_register(L, "WriteMemory", LuaWriteMemory);
    lua_register(L, "PatternScan", LuaPatternScan);
    lua_register(L, "AllocateMemory", LuaAllocateMemory);
    lua_register(L, "WriteMemoryArray", LuaWriteMemoryArray);
}
