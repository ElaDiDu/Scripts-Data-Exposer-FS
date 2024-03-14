#pragma once

#include <Windows.h>
#include <Psapi.h>

struct ProcessInfo 
{
    HANDLE hProcess;
    HMODULE hModule;
    MODULEINFO mInfo;
    bool init = false;
};

static ProcessInfo PROCESS_INFO;

typedef void HksState;



static void initBase()
{
    PROCESS_INFO.init = true;
    PROCESS_INFO.hProcess = GetCurrentProcess();
    PROCESS_INFO.hModule = GetModuleHandleA(NULL);
    GetModuleInformation(PROCESS_INFO.hProcess, PROCESS_INFO.hModule, &PROCESS_INFO.mInfo, sizeof(MODULEINFO));
}

intptr_t getProcessBase()
{
    if (!PROCESS_INFO.init) initBase();

    return (intptr_t)PROCESS_INFO.mInfo.lpBaseOfDll;
}


//hks functions

//+ 0x040de30
/*
* Original hks "env" function.
*/
int (*hksEnv)(void** chrInsPtr, int envId, HksState* hksState);
void* replacedHksEnv;

void (*hksAct)(void** chrInsPtr, int actId, HksState* hksState);
void* replacedHksAct;


//+ 0x149e6a0
int (*hks_lua_type)(HksState* hksState, int idx);

//+ 0x140B910
/*
* Tests if function to be executed has an nth param.
*/
bool* (*hksHasParamIntOut)(bool* out, void* hksState, int paramIndex);

//bool (*hksHasParam)(void* hksState, int paramIndex);

//+ 0x14A32C0
/*
* Gets the function to be executed's nth param as an int.
*/
int (*hks_luaL_checkint)(HksState* hksState, int paramIndex);

//+ 0xbce940
/*
* Returns the function to be executed's nth param as an int if it exists, otherwise default value.
*/
int (*hks_luaL_checkoptint)(HksState* hksState, int paramIndex, int defaultVal);

//+ 0x14A3370
/*
* Gets the function to be executed's nth param as a float.
*/
float (*hks_luaL_checknumber)(HksState* hksState, int paramIndex);

//+ 0x1497180
char* (*hks_luaL_checklstring)(HksState* hksState, int paramIndex, int* lenOut);


bool hksHasParamInt(HksState* hksState, int paramIndex)
{
    bool out = false;
    return *hksHasParamIntOut(&out, hksState, paramIndex);
}


//TODO not hks specific funcs, move somewhere else
void** VirtualMemoryFlag;

bool (*getEventFlag)(void* virtualMemoryFlag, unsigned int flagId);

void (*setEventFlag)(void* virtualMemoryFlag, unsigned int flagId, int val);