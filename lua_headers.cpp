#include "lua_headers.h"

const char* coordinates_script = R"(
local function userdata_to_number(ud)
    local ud_str = tostring(ud)
    local hex_str = ud_str:match("%x+")
    if hex_str then
        return tonumber(hex_str, 16)
    else
        return nil
    end
end

local function nopAOB(moduleName, pattern, length)
    local address = PatternScan(moduleName, pattern)
    if address == 0 then
        print("Pattern not found.")
        return false
    end
    print("Pattern found at address: " .. string.format("0x%X", address))
    
    -- Create a string of NOPs, using the hexadecimal escape for NOP, which is \x90
    local nopString = string.rep("\x90", length)
    
    if WriteMemoryArray(address, nopString, length) then
        print("NOP'd the AOB successfully.")
        return true
    else
        print("Failed to NOP the AOB.")
        return false
    end
end

-- Call the function with necessary parameters
nopAOB("ChainedTogether-Win64-Shipping.exe", "E8 3D 5E FF FF", 5)

)";

const char* hello_world_script = R"(
-- hello_world.lua
print("Hello, World from Lua!")

)";

