#pragma once

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

extern lua_State* L;  

void InitializeLua();
void ExecuteLuaScript(const char* script);
void ExecuteLuaString(const char* luaCode);
void CloseLua();

int LuaReadMemory(lua_State* L);
int LuaWriteMemory(lua_State* L);
int LuaPatternScan(lua_State* L);
void RegisterLuaFunctions(lua_State* L);
