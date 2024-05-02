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

#define OPEN_CONSOLE_ON_START 0


static void initAddresses()
{
    GetText();
    ScanAndAssignAddresses();
}


bool createHook(LPVOID pTarget, LPVOID pDetour, LPVOID* ppOriginal)
{
    int mhStatus = MH_CreateHook(pTarget, pDetour, ppOriginal);
    if (mhStatus != MH_OK)
    {
        Logger::log("MinHook CreateHook error creating hook (%d)", mhStatus);
        return false;
    }
    return true;
}

void initHooks() 
{
    createHook(replacedHksEnv, &envHookFunc, (void**)&hksEnv);
    createHook(replacedHksAct, &actHookFunc, (void**)&hksAct);

    MH_EnableHook(MH_ALL_HOOKS);
}

void onAttach()
{
    if (OPEN_CONSOLE_ON_START && GetConsoleWindow() == NULL) 
    {
        AllocConsole();
        freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
        Logger::log("Created Scripts-Data-Exposer-FS Console");

    }

    Logger::log("Start onAttach");

    initBase();

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

