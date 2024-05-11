// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <Windows.h>
#include <iostream>
#include <vector>
#include <Psapi.h>
#include "game/HksState.h"
#include "include/MinHook.h"
#include "include/Logger.h"
#include "game/ProcessData.h"
#include "game/AOBScan.h"


#if _WIN64
#pragma comment(lib, "libMinHook-x64-v141-md.lib")
#else
#pragma comment(lib, "libMinHook-x86-v141-md.lib")
#endif

#if _DEBUG
#define DEBUG true
#else
#define DEBUG false
#endif

#if DEBUG
static inline void printAddresses() 
{
    Logger::debug("---");
    Logger::debug("Addresses:");
    for (auto const& [name, address] : registeredAddresses) 
    {
        Logger::debug("%s: %p", name, address);
    }
    Logger::debug("%s: %p", "newEnvFunc", newEnvFunc);
    Logger::debug("%s: %p", "newActFunc", newActFunc);
    Logger::debug("---");
}
#else
static inline void printAddresses() {};
#endif


static void initAddresses()
{
    GetText();
    ScanAndAssignAddresses();
    printAddresses();
}


bool createHook(const char* name, LPVOID pTarget, LPVOID pDetour, LPVOID* ppOriginal)
{
    int mhStatus = MH_CreateHook(pTarget, pDetour, ppOriginal);
    if (mhStatus != MH_OK)
    {
        Logger::log("MinHook CreateHook error creating hook \"%s\" (%d)", name, mhStatus);
        return false;
    }
    return true;
}

void initHooks() 
{
    createHook("hksSetCGlobals", replacedHksSetCGlobals, &hksSetCGlobalsHookFunc, (void**)&hksSetCGlobals);

    MH_EnableHook(MH_ALL_HOOKS);
}

void onAttach()
{
    if (DEBUG && GetConsoleWindow() == NULL) 
    {
        AllocConsole();
        freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
        Logger::debug("Created Scripts-Data-Exposer-FS Console");

    }

    Logger::debug("Start onAttach");

    initBase();

    int mhStatus = MH_Initialize();
    if (mhStatus != MH_OK) 
    {
        Logger::log("MinHok Initialize error " + mhStatus);
        return;
    }

    initAddresses();

    initHooks();

    Logger::debug("Finished onAttach");
}

void onDetach() 
{
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
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
        onDetach();
        break;
    }
    return TRUE;
}

