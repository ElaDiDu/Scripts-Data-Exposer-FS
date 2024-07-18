#pragma once

#include <Windows.h>
#include <Psapi.h>
#include "ProcessStructs.h"
#include "PointerChain.h"

struct ProcessInfo 
{
    HANDLE hProcess;
    HMODULE hModule;
    MODULEINFO mInfo;
    bool init = false;
};

static ProcessInfo PROCESS_INFO;


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


//bases
void** VirtualMemoryFlag;
void** SoloParamRepository;
void** WorldChrMan;

//functions

static const char* CALLER_NAME = "ExposerEventCaller";

//misc
bool (*getEventFlagPtr)(void* virtualMemoryFlag, unsigned int* flagId, EventFlagCaller* caller);

bool getEventFlag(void* virtualMemoryFlag, unsigned int flagId) 
{
    EventFlagCaller caller;
    caller.caller = (void*)CALLER_NAME;
    return getEventFlagPtr(virtualMemoryFlag, &flagId, &caller);
}

void (*setEventFlagPtr)(void* virtualMemoryFlag, unsigned int* flagId, bool val, EventFlagCaller* caller, bool unk_5);

void setEventFlag(void* virtualMemoryFlag, unsigned int flagId, int val) 
{
    EventFlagCaller caller;
    caller.caller = (void*)CALLER_NAME;
    setEventFlagPtr(virtualMemoryFlag, &flagId, val != 0, &caller, true);
}

void* (*getParamResCap)(void* soloParamRepository, int paramIndex, int unk);

inline void* getParamData(int paramIndex) 
{
    intptr_t param = (intptr_t)getParamResCap(*SoloParamRepository, paramIndex, 0);
    if (param == NULL) return NULL;
    return (void*)(*(intptr_t*)((*(intptr_t*)(param + 0x80)) + 0x80));
}

void* (*getChrInsFromHandle)(void* worldChrMan, uint64_t* handlePtr);

char (*replaceItem)(void* equipGameData, int itemToReplace, int newItem, char unk3);


//HKS
//+ 0x040de30
/*
* Original hks "env" function.
*/
int (*hksEnv)(void** chrInsPtr, int envId, HksState* hksState);
void* replacedHksEnv;

int (*hksAct)(void** chrInsPtr, int actId, HksState* hksState);
void* replacedHksAct;


//+ 0x149e6a0
int (*hks_lua_type)(HksState* hksState, int idx);

//+ 0x140B910
/*
* Tests if function to be executed has an nth param.
*/
bool* (*hksHasParamNumberOut)(bool* out, void* hksState, int idx);

//bool (*hksHasParam)(void* hksState, int idx);

//+ 0x14A32C0
/*
* Gets the function to be executed's nth param as an int.
*/
int (*hks_luaL_checkint)(HksState* hksState, int idx);

//+ 0xbce940
/*
* Returns the function to be executed's nth param as an int if it exists, otherwise default value.
*/
int (*hks_luaL_checkoptint)(HksState* hksState, int idx, int defaultVal);

//+ 0x14A3370
/*
* Gets the function to be executed's nth param as a float.
*/
float (*hks_luaL_checknumber)(HksState* hksState, int idx);

//+ 0x1497180
const char* (*hks_luaL_checklstring)(HksState* hksState, int idx, size_t* lenOut);

/*
* Push number onto the lua stack. (E.g use to return a number in a CFunction).
*/
void (*hks_lua_pushnumber)(HksState* hksState, float number);

/*
* Gets hkbCharacter (ptr) owner of the HKS script. This probably returns a pointer to a whole struct whose first element is hkbChr. hkbCharacter->28 is ChrIns
*/
void** (*getHkbChrFromHks)(HksState* hksState);

/*
* Pushes a few globals onto the state, including env and act
*/
void (*hksSetCGlobals)(HksState* hksState);
void* replacedHksSetCGlobals;

/*
* Adds a CFunction to the global table
*/
void (*hks_addnamedcclosure)(HksState* hksState, const char* name, void* func);


bool hksHasParamNumber(HksState* hksState, int paramIndex)
{
    bool out = false;
    return *hksHasParamNumberOut(&out, hksState, paramIndex);
}

void* getHksChrInsOwner(HksState* hksState) 
{
    intptr_t hkbCharacter = (intptr_t)*getHkbChrFromHks(hksState);
    if (hkbCharacter == NULL) return NULL;

    return *(void **)(hkbCharacter + 0x28);
}

/*
* Gets the function to be executed's nth param as a string.
*/
const char* hksGetParamString(HksState* hksState, int paramIndex)
{
    return hks_luaL_checklstring(hksState, paramIndex, NULL);
}

std::string hksParamToString(HksState* hksState, int paramIndex)
{
    int vtype = hks_lua_type(hksState, paramIndex);
    if (vtype == LUA_TSTRING)
        return hksGetParamString(hksState, paramIndex);
    if (vtype == LUA_TNUMBER)
        return std::to_string(hks_luaL_checknumber(hksState, paramIndex)).data();
    if (vtype == LUA_TNONE || vtype == LUA_TNIL)
        return "nil";

    return "Object Type " + vtype;
}

/*
* Same as checkint but if the param is string then convert into long
*/
bool hksGetParamLong(HksState* hksState, int paramIndex, long long& result)
{
    int type = hks_lua_type(hksState, paramIndex);

    if (type == LUA_TNUMBER)
    {
        result = hks_luaL_checknumber(hksState, paramIndex);
        return true;
    }
    else if (type == LUA_TSTRING)
    {
        const char* str = hks_luaL_checklstring(hksState, paramIndex, NULL);
        char* endptr;
        result = strtol(str, &endptr, 10);
        if (endptr == str)
            result = strtod(str, &endptr);
        if (endptr == str) /* conversion failed */
            return false;
        if (*endptr == 'x' || *endptr == 'X')  /* maybe an hexadecimal constant? */
            result = strtoul(str, &endptr, 16);
        if (*endptr == '\0') /* most common case */
            return true;
        while (isspace((unsigned char)*endptr)) endptr++;
        if (*endptr != '\0') /* invalid trailing characters? */
            return false;

        return true;
    }

    return false;
}

/*
* Same as checkint but if the param is string then convert into long
*/
inline long long hksGetParamLong(HksState* hksState, int paramIndex, bool& valid)
{
    long long result = 0;
    valid = hksGetParamLong(hksState, paramIndex, result);
    return result;
}

/*
* Same as checkint but if the param is string then convert into long
*/
inline long long hksGetParamLong(HksState* hksState, int paramIndex)
{
    long long result = 0;
    hksGetParamLong(hksState, paramIndex, result);
    return result;
}

/*
* Same as checkint but if the param is string then convert into int. Bypasses lua's native float conversion first.
*/
inline int hksGetParamInt(HksState* hksState, int paramIndex, bool& valid) 
{
    return (int)hksGetParamLong(hksState, paramIndex, valid);
}

/*
* Same as checkint but if the param is string then convert into int. Bypasses lua's native float conversion first.
*/
inline int hksGetParamInt(HksState* hksState, int paramIndex)
{
    bool valid;
    return (int)hksGetParamInt(hksState, paramIndex, valid);
}

/*
* Same as checknumber but if the param is string then convert into double
*/
bool hksGetParamDouble(HksState* hksState, int paramIndex, double& result)
{
    int type = hks_lua_type(hksState, paramIndex);

    if (type == LUA_TNUMBER)
    {
        result = hks_luaL_checknumber(hksState, paramIndex);
        return true;
    }
    else if (type == LUA_TSTRING)
    {
        const char* str = hks_luaL_checklstring(hksState, paramIndex, NULL);
        char* endptr;
        result = strtod(str, &endptr);
        if (endptr == str) /* conversion failed */
            return false;
        if (*endptr == 'x' || *endptr == 'X')  /* maybe an hexadecimal constant? */
            result = strtoul(str, &endptr, 16);
        if (*endptr == '\0') /* most common case */
            return true;
        while (isspace((unsigned char)*endptr)) endptr++;
        if (*endptr != '\0') /* invalid trailing characters? */
            return false;

        return true;
    }

    return false;
}

/*
* Same as checknumber but if the param is string then convert into double
*/
inline double hksGetParamDouble(HksState* hksState, int paramIndex, bool& valid)
{
    double result = 0;
    valid = hksGetParamDouble(hksState, paramIndex, result);
    return result;
}

/*
* Same as checknumber but if the param is string then convert into double
*/
inline double hksGetParamDouble(HksState* hksState, int paramIndex)
{
    double result = 0;
    hksGetParamDouble(hksState, paramIndex, result);
    return result;
}



//EMEVD
bool (*emevdSystemCondition)(void* unk1, void* unk2, CSEmkEventIns* event);

bool (*emevdSystemControlFlow)(void* unk1, void* unk2, CSEmkEventIns* event);

bool (*emevdSystemFunction)(void* unk1, void* unk2, CSEmkEventIns* event);


//ESD

//GAME DEBUG

//Creates a new chr using the debug methods. This chr will have to be manually deleted for memory management.
void createChrDebug(ChrSpawnDbgProperties& properties) 
{
    intptr_t dbgChrCreator = *PointerChain::make<intptr_t, true>(WorldChrMan, 0x1e640);
    if (dbgChrCreator == NULL) return;

    *(wchar_t*)(dbgChrCreator + 0x100) = properties.model[0];
    *(wchar_t*)(dbgChrCreator + 0x102) = properties.model[1];
    *(wchar_t*)(dbgChrCreator + 0x104) = properties.model[2];
    *(wchar_t*)(dbgChrCreator + 0x106) = properties.model[3];
    *(wchar_t*)(dbgChrCreator + 0x108) = properties.model[4];
    *(wchar_t*)(dbgChrCreator + 0x10a) = 0;

    *(bool*)(dbgChrCreator + 0x178) = properties.isPlayer;

    *(int*)(dbgChrCreator + 0xf0) = properties.npcParam;
    *(int*)(dbgChrCreator + 0xf4) = properties.npcThinkParam;
    *(int*)(dbgChrCreator + 0xf8) = properties.eventEntityId;
    *(int*)(dbgChrCreator + 0xfc) = properties.talkId;

    *(int*)(dbgChrCreator + 0x17c) = properties.charaInitParam;
    *(int*)(dbgChrCreator + 0x180) = properties.manipulatorType;

    *(float*)(dbgChrCreator + 0xb0) = properties.posX;
    *(float*)(dbgChrCreator + 0xb4) = properties.posY;
    *(float*)(dbgChrCreator + 0xb8) = properties.posZ;


    *(bool*)(dbgChrCreator + 0x44) = true; //Set spawn = true
}

//Delete chr (use for manual deletion of manually created chrs)
void (*deleteChr)(void* worldChrMan, void* chrIns);

void* getLatestDebugChr() 
{
    return *PointerChain::make<void*, true>(WorldChrMan, 0x1e640, 0x1b0);
}
