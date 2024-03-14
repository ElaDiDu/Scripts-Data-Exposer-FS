// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <Windows.h>
#include <iostream>
#include <vector>
#include <Psapi.h>
#include "HksState.h"
#include "include/MinHook.h"
#include "Logger.h"
#include "ProcessData.h"
#include "AOBScan.h"


#if _WIN64
#pragma comment(lib, "libMinHook-x64-v141-md.lib")
#else
#pragma comment(lib, "libMinHook-x86-v141-md.lib")
#endif

#define OPEN_CONSOLE_ON_START 0


static inline void** getAbsoluteAddressBase(intptr_t base, intptr_t offset)
{
    return reinterpret_cast<void**>(base + offset);
}

static inline void** getAbsoluteAddressBase(intptr_t base, void* offset)
{
    return reinterpret_cast<void**>(base + (intptr_t)offset);
}

static inline void* getAbsoluteAddressFunc(intptr_t base, intptr_t offset)
{
    return reinterpret_cast<void*>(base + offset);
}

static inline void* getAbsoluteAddressFunc(intptr_t base, void* offset)
{
    return reinterpret_cast<void*>(base + (intptr_t)offset);
}

static void initAddresses()
{
    intptr_t process = getProcessBase();

    GetText();

    //AOBs
    const unsigned char gameDataManAOB[] = { 0x48, 0x8B, 0x05, 0x0, 0x0, 0x0, 0x0, 0x48, 0x85, 0xC0, 0x74, 0x05, 0x48, 0x8B, 0x40, 0x58, 0xC3, 0xC3, };
    const char* gameDataManMask = "...????...........";
    const unsigned char virtualMemoryFlagAOB[] = { 0x48, 0x8B, 0x3D, 0x0, 0x0, 0x0, 0x0, 0x48, 0x85, 0xFF, 0x74, 0x0, 0x48, 0x8B, 0x49, };
    const char* virtualMemoryFlagMask = "...????....?...";

    const unsigned char hksEnvAOB[] = { 0x48, 0x8b, 0xc4, 0x55, 0x56, 0x57, 0x41, 0x54, 0x41, 0x55, 0x41, 0x56, 0x41, 0x57, 0x48, 0x8d, 0x68, 0xa1, 0x48, 0x81, 0xec, 0xe0, 0x00, 0x00, 0x00, 0x48, 0xc7, 0x45, 0xb7, 0xfe, 0xff, 0xff, 0xff, };
    const char* hksEnvMask = ".................................";
    const unsigned char hksActAOB[] = { 0x48, 0x8b, 0xc4, 0x55, 0x56, 0x57, 0x41, 0x56, 0x41, 0x57, 0x48, 0x8d, 0x68, 0xa8, 0x48, 0x81, 0xec, 0x30, 0x01, 0x00, 0x00, 0x48, 0xc7, 0x44, 0x24, 0x50, 0xfe, 0xff, 0xff, 0xff, };
    const char* hksActMask = "..............................";
    const unsigned char hks_lua_typeAOB[] = { 0x41, 0x8b, 0x10, 0xb8, 0x06, 0x00, 0x00, 0x00, 0x83, 0xe2, 0x0f, 0x8d, 0x4a, 0xf7, 0x83, 0xf9, 0x01, 0x0f, 0x47, 0xc2, 0x48, 0x83, 0xc4, 0x30, 0x5b, 0xc3, };
    const char* hks_lua_typeMask = "..........................";
    const unsigned char hksHasParamIntOutAOB[] = { 0x48, 0x89, 0x5c, 0x24, 0x08, 0x48, 0x89, 0x74, 0x24, 0x10, 0x57, 0x48, 0x83, 0xec, 0x20, 0x41, 0x8b, 0xd8, 0x8b, 0xfa, 0x44, 0x8b, 0xc2, 0x48, 0x8b, 0xf1, 0x48, 0x8b, 0xd1, 0x48, 0x8d, 0x4c, 0x24, 0x48, 0xe8, 0x0, 0x0, 0x0, 0x0, 0x80, 0x38, 0x00, };
    const char* hksHasParamIntOutMask = "...................................????...";
    const unsigned char hks_luaL_checkintAOB[] = { 0x8b, 0xd6, 0x48, 0x8b, 0xcf, 0xe8, 0x0, 0x0, 0x0, 0x0, 0x8b, 0xc3, 0x48, 0x8b, 0x5c, 0x24, 0x40, 0x48, 0x8b, 0x74, 0x24, 0x48, 0x48, 0x83, 0xc4, 0x30, 0x5f, 0xc3, };
    const char* hks_luaL_checkintMask = "......????..................";
    const unsigned char hks_luaL_checknumberAOB[] = { 0x8b, 0xd7, 0x48, 0x8b, 0xcb, 0xe8, 0x0, 0x0, 0x0, 0x0, 0x0f, 0x28, 0xf0, 0x0f, 0x57, 0xc9, 0x0f, 0x2e, 0xf1, 0x75, 0x0, 0x8b, 0xd7, 0x48, 0x8b, 0xcb, };
    const char* hks_luaL_checknumberMask = "......????..........?.....";
    const unsigned char hks_luaL_checklstringAOB[] = { 0x4c, 0x8b, 0xc3, 0x8b, 0xd6, 0x48, 0x8b, 0xcf, 0xe8, 0x0, 0x0, 0x0, 0x0, 0x48, 0x8b, 0xd8, 0x48, 0x85, 0xc0, 0x75, };
    const char* hks_luaL_checklstringMask = ".........????.......";
    const unsigned char getEventFlagAOB[] = { 0x44, 0x8b, 0xda, 0x33, 0xd2, 0x41, 0x8b, 0xc3, 0x41, 0xf7, 0xf0, 0x4c, 0x8b, 0xd1, 0x45, 0x33, 0xc9, 0x44, 0x0f, 0xaf, 0xc0, 0x45, 0x2b, 0xd8, };
    const char* getEventFlagMask = "........................";
    const unsigned char setEventFlagAOB[] = { 0x44, 0x8b, 0xd2, 0x33, 0xd2, 0x41, 0x8b, 0xc2, 0x41, 0xf7, 0xf1, 0x41, 0x8b, 0xd8, 0x4c, 0x8b, 0xd9, };
    const char* setEventFlagMask = ".................";

    //Globals
    //VirtualMemoryFlag = (void **)getAbsoluteAddress(process, 0x3cdf238);

    VirtualMemoryFlag = AOBScanBase(virtualMemoryFlagAOB, virtualMemoryFlagMask);


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

    hksEnv = (int(*)(void**, int, HksState*))AOBScanCode(hksEnvAOB, hksEnvMask);
    replacedHksEnv = hksEnv;
    hksAct = (void(*)(void**, int, HksState*))AOBScanCode(hksActAOB, hksActMask);
    replacedHksAct = hksAct;
    hks_lua_type = (int(*)(HksState*, int))AOBScanCode(hks_lua_typeAOB, hks_lua_typeMask, -257);
    hksHasParamIntOut = (bool* (*)(bool*, HksState*, int))AOBScanCodeCall(hksHasParamIntOutAOB, hksHasParamIntOutMask, 34, 34);
    hks_luaL_checkint = (int(*)(HksState*, int))AOBScanCode(hks_luaL_checkintAOB, hks_luaL_checkintMask, -123);
    hks_luaL_checknumber = (float(*)(HksState*, int))AOBScanCode(hks_luaL_checknumberAOB, hks_luaL_checknumberMask, -86);
    hks_luaL_checklstring = (char* (*)(HksState*, int, int*))AOBScanCode(hks_luaL_checklstringAOB, hks_luaL_checklstringMask, -89);
    getEventFlag = (bool (*)(void*, unsigned int))AOBScanCode(getEventFlagAOB, getEventFlagMask, -4);
    setEventFlag = (void (*)(void*, unsigned int, int))AOBScanCode(setEventFlagAOB, setEventFlagMask, -9);
}


bool createHook(LPVOID pTarget, LPVOID pDetour, LPVOID* ppOriginal)
{
    int mhStatus = MH_CreateHook(pTarget, pDetour, ppOriginal);
    if (mhStatus != MH_OK)
    {
        Logger::log("MinHook CreateHook error creating hook " + std::to_string(mhStatus));
        return false;
    }
    return true;
}

void initHooks() 
{
    createHook(replacedHksEnv, &envHookFunc, (void**)&hksEnv);
    createHook(replacedHksAct, &actHookFunc, (void**)&hksAct);

    MH_EnableHook(NULL);
}

void onAttach()
{
    if (OPEN_CONSOLE_ON_START && GetConsoleWindow() == NULL) 
    {
        AllocConsole();
        freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
        Logger::log("Created Scripts-Data-Exposer-FS Console");

    }
    /*if (GetConsoleWindow() == NULL)
    {
        AllocConsole();
        freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
        Logger::log("Created Scripts-Data-Exposer-FS Console");
    }*/
    Logger::log("Start onAttach");

    int mhStatus = MH_Initialize();
    if (mhStatus != MH_OK) 
    {
        Logger::log("MinHok Initialize error " + mhStatus);
        return;
    }

    
    initAddresses();

    initHooks();


    Logger::log("Finished onAttach");
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        onAttach();
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

