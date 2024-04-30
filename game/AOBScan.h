#pragma once
#include "../include/mem/mem.h"
#include "../include/mem/pattern.h"
#include "../include/mem/simd_scanner.h"
#include "../include/Logger.h"
#include "ProcessData.h"
#include <iomanip>
#include <sstream>

static void* text;
static size_t text_size;


//Regular AOB scan
inline void* AOBScanAddress(const char* AOBString, const void* region = text, const size_t region_size = text_size) 
{
    const auto pattern = mem::pattern(AOBString);
    const auto mregion = mem::region(region, region_size);
    char* result = mem::simd_scanner(pattern).scan(mregion).any();

    if (!result) 
    {
        Logger::log("AOB string \"%s\" not found.", AOBString);
    }

    return reinterpret_cast<void*>(result);
}


//Regular AOB scan
inline void* AOBScanAddress(const unsigned char* AOBString, const char* AOBMask, const void* region = text, const size_t region_size = text_size)
{
    const auto pattern = mem::pattern(AOBString, AOBMask);
    const auto mregion = mem::region(region, region_size);
    char* result = mem::simd_scanner(pattern).scan(mregion).any();

    if (!result)
    {
        size_t AOBStrLen = std::strlen(AOBMask);
        std::stringstream str;
        str << "AOB string ";
        for (size_t i = 0; i < AOBStrLen; i++)
        {
            str << std::hex << static_cast<int>(AOBString[i]) << " ";
        }
        str << "not found.";
        Logger::log(str.str());
    }

    return reinterpret_cast<void*>(result);
}

//AOB scan with offset (e.g for when the AOB is inside the function)
inline void* AOBScanCode(const char* AOBString, const int Offset = 0, const void* region = text, const size_t region_size = text_size)
{
    uint8_t* addr = reinterpret_cast<uint8_t*>(AOBScanAddress(AOBString, region, region_size));

    if (!addr)
    {
        return nullptr;
    }

    return reinterpret_cast<void*>(addr + Offset);
}

//AOB scan with offset (e.g for when the AOB is inside the function)
inline void* AOBScanCode(const uint8_t* AOBString, const char* AOBMask, const int Offset = 0, const void* region = text, const size_t region_size = text_size)
{
    uint8_t* addr = reinterpret_cast<uint8_t*>(AOBScanAddress(AOBString, AOBMask, region, region_size));

    if (!addr)
    {
        return nullptr;
    }

    return reinterpret_cast<void*>(addr + Offset);
}

//AOB scan for a global pointer [base] through an instruction that uses it
inline void** AOBScanBase(const char* AOBString, const int InOffset = 7, const int OpOffset = 3)
{
    uint8_t* addr = static_cast<uint8_t*>(AOBScanAddress(AOBString));
    return addr != nullptr ? reinterpret_cast<void**>(addr + *reinterpret_cast<int32_t*>(addr + OpOffset) + InOffset) : nullptr;
}

//AOB scan for a global pointer [base] through an instruction that uses it
inline void** AOBScanBase(const unsigned char* AOBString, const char* AOBMask, const int InOffset = 7, const int OpOffset = 3) 
{
    uint8_t* addr = static_cast<uint8_t*>(AOBScanAddress(AOBString, AOBMask));
    return addr != nullptr ? reinterpret_cast<void**>(addr + *reinterpret_cast<int32_t*>(addr + OpOffset) + InOffset) : nullptr;
}

inline void* AOBScanFuncCall(const unsigned char* AOBString1, const char* AOBMask1, const unsigned char* AOBString2, const char* AOBMask2, const size_t FuncSize, const int CallOffset)
{
    uint8_t* addr = reinterpret_cast<uint8_t*>(AOBScanAddress(AOBString1, AOBMask1));

    if (!addr)
    {
        return nullptr;
    }

    addr = addr + *reinterpret_cast<int32_t*>(addr + 1 + CallOffset) + 5 + CallOffset;
    return AOBScanAddress(AOBString2, AOBMask2, addr, FuncSize);
}

//AOB scan for a function through a different function that calls it
inline void* AOBScanCodeCall(const char* AOBString, const int FuncStartToOpOffset, const int OpOffset)
{
    uint8_t* addr = static_cast<uint8_t*>(AOBScanAddress(AOBString));
    return addr != nullptr ? reinterpret_cast<void*>(addr + *reinterpret_cast<int32_t*>(addr + OpOffset + 1) + 5 + FuncStartToOpOffset) : nullptr;
}

//AOB scan for a function through a different function that calls it
inline void* AOBScanCodeCall(const unsigned char* AOBString, const char* AOBMask, const int FuncStartToOpOffset, const int OpOffset)
{
    uint8_t* addr = static_cast<uint8_t*>(AOBScanAddress(AOBString, AOBMask));
    return addr != nullptr ? reinterpret_cast<void*>(addr + *reinterpret_cast<int32_t*>(addr + OpOffset + 1) + 5 + FuncStartToOpOffset) : nullptr;
}

extern void GetText()
{
    const char textStrMatch[] = ".text"; // refused to work with char* string

    const auto pattern = mem::pattern(textStrMatch, ".....");
    const auto region = mem::region(PROCESS_INFO.mInfo.lpBaseOfDll, PROCESS_INFO.mInfo.SizeOfImage);
    uint8_t* result = mem::simd_scanner(pattern).scan(region).any();

    text_size = *reinterpret_cast<uint32_t*>(result + 0x10);
    text = result + *reinterpret_cast<uint32_t*>(result + 0x14);
};

extern void ScanAndAssignAddresses() 
{
    const char* gameDataManAOB = "48 8b 05 ?? ?? ?? ?? 48 85 c0 74 05 48 8b 40 58 c3 c3";
    const char* virtualMemoryFlagAOB = "48 8b 3d ?? ?? ?? ?? 48 85 ff 74 ?? 48 8b 49";
    const char* soloParamRepositoryAOB = "8b da 4c 8b f9 45 33 ed 45 33 e4 48 8b 0d";
    const char* worldChrManAOB = "48 8b 05 ?? ?? ?? ?? 48 85 c0 74 0f 48 39 88";


    const char* hksEnvAOB = "48 8b c4 55 56 57 41 54 41 55 41 56 41 57 48 8d 68 a1 48 81 ec e0 00 00 00 48 c7 45 b7 fe ff ff ff";
    const char* hksActAOB = "48 8b c4 55 56 57 41 56 41 57 48 8d 68 a8 48 81 ec 30 01 00 00 48 c7 44 24 50 fe ff ff ff";
    const char* hks_lua_typeAOB = "41 8b 10 b8 06 00 00 00 83 e2 0f 8d 4a f7 83 f9 01 0f 47 c2 48 83 c4 30 5b c3";
    const char* hksHasParamIntOutAOB = "48 89 5c 24 08 48 89 74 24 10 57 48 83 ec 20 41 8b d8 8b fa 44 8b c2 48 8b f1 48 8b d1 48 8d 4c 24 48 e8 ?? ?? ?? ?? 80 38 00";
    const char* hks_luaL_checkintAOB = "8b d6 48 8b cf e8 ?? ?? ?? ?? 8b c3 48 8b 5c 24 40 48 8b 74 24 48 48 83 c4 30 5f c3";
    const char* hks_luaL_checknumberAOB = "8b d7 48 8b cb e8 ?? ?? ?? ?? 0f 28 f0 0f 57 c9 0f 2e f1 75 ?? 8b d7 48 8b cb";
    const char* hks_luaL_checklstringAOB = "4c 8b c3 8b d6 48 8b cf e8 ?? ?? ?? ?? 48 8b d8 48 85 c0 75";
    const char* getEventFlagAOB = "44 8b da 33 d2 41 8b c3 41 f7 f0 4c 8b d1 45 33 c9 44 0f af c0 45 2b d8";
    const char* setEventFlagAOB = "44 8b d2 33 d2 41 8b c2 41 f7 f1 41 8b d8 4c 8b d9";
    const char* replaceItemAOB = "40 57 48 83 ec 40 48 c7 44 24 30 fe ff ff ff 48 89 5c 24 60 48 89 74 24 68 41 0f b6 f1 41 8b d8 48 8b f9 81 e2 ff ff ff 0f";
    const char* getParamResCapAOB = "48 63 d2 48 8d 04 d2 44 3b 84 c1 80 00 00 00";
    const char* getChrInsFromHandleAOB = "48 83 ec 28 e8 ?? ?? ?? ?? 48 85 c0 74 ?? 48 8b 00 48 83 c4 28 c3 48 83 c4 28 c3";


    //Globals
    //VirtualMemoryFlag = (void **)getAbsoluteAddress(process, 0x3cdf238);

    VirtualMemoryFlag = AOBScanBase(virtualMemoryFlagAOB);
    SoloParamRepository = AOBScanBase(soloParamRepositoryAOB, 18, 14);
    WorldChrMan = AOBScanBase(worldChrManAOB);

    //Functions
    //hksEnv = (int(*)(void**, int, HksState*))getAbsoluteAddressFunc(process, 0x040de30);
    //hksAct = (void(*)(void**, int, HksState*))getAbsoluteAddressFunc(process, 0x040a1e0);
    //hks_lua_type = (int(*)(HksState*, int))getAbsoluteAddressFunc(process, 0x149e6a0);
    //hksHasParamIntOut = (bool* (*)(bool*, HksState*, int))getAbsoluteAddressFunc(process, 0x13f26f0);
    //hks_luaL_checkint = (int(*)(HksState*, int))getAbsoluteAddressFunc(process, 0x14A32C0);
    //hks_luaL_checkoptint = (int(*)(HksState*, int, int))getAbsoluteAddressFunc(process, 0xbce940);
    //hks_luaL_checknumber = (float(*)(HksState*, int))getAbsoluteAddressFunc(process, 0x14a3370);
    //hks_luaL_checklstring = (char* (*)(HksState*, int, int*))getAbsoluteAddressFunc(process, 0x1497180);
    //getEventFlag = (bool (*)(void*, unsigned int))getAbsoluteAddressFunc(process, 0x05edc20);
    //setEventFlag = (void (*)(void*, unsigned int, int))getAbsoluteAddressFunc(process, 0x05ee410);

    hksEnv = (int(*)(void**, int, HksState*))AOBScanCode(hksEnvAOB);
    replacedHksEnv = hksEnv;
    hksAct = (void(*)(void**, int, HksState*))AOBScanCode(hksActAOB);
    replacedHksAct = hksAct;
    hks_lua_type = (int(*)(HksState*, int))AOBScanCode(hks_lua_typeAOB, -257);
    hksHasParamIntOut = (bool* (*)(bool*, HksState*, int))AOBScanCodeCall(hksHasParamIntOutAOB, 34, 34);
    hks_luaL_checkint = (int(*)(HksState*, int))AOBScanCode(hks_luaL_checkintAOB, -123);
    hks_luaL_checknumber = (float(*)(HksState*, int))AOBScanCode(hks_luaL_checknumberAOB, -86);
    hks_luaL_checklstring = (char* (*)(HksState*, int, int*))AOBScanCode(hks_luaL_checklstringAOB, -89);
    getEventFlag = (bool (*)(void*, unsigned int))AOBScanCode(getEventFlagAOB, -4);
    setEventFlag = (void (*)(void*, unsigned int, int))AOBScanCode(setEventFlagAOB, -9);
    replaceItem = (char (*)(int, int, char))AOBScanCode(replaceItemAOB, 0);
    getParamResCap = (void* (*)(void*, int, int))AOBScanCode(getParamResCapAOB, -8);
    getChrInsFromHandle = (void* (*)(void*, uint64_t*))AOBScanCode(getChrInsFromHandleAOB, 0);
}