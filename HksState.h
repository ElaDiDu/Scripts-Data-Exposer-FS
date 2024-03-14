#pragma once
#include <string>
#include "ProcessData.h"

void exposerLog(std::string);
intptr_t getProcessBase();

enum EnvId 
{
    TRAVERSE_POINTER_CHAIN = 10000,
    EXECUTE_FUNCTION = 10002, 
    GET_EVENT_FLAG = 10003
};
enum ActId 
{ 
    WRITE_POINTER_CHAIN = 10000, 
    DEBUG_PRINT = 10001, 
    UPDATE_MAGICID = 10002, 
    SET_EVENT_FLAG = 10003
};
enum PointerBaseType { GAME = 0, CHR_INS = 1};
enum WritePointerChainType 
{
    SET_UNSIGNED_BYTE = 0,
    SET_SIGNED_BYTE = 1,
    SET_UNSIGNED_SHORT = 2,
    SET_SIGNED_SHORT = 3,
    SET_UNSIGNED_INT = 4,
    SET_SIGNED_INT = 5,
    SET_FLOAT = 6,
};


//hks functions return invalid when something is wrong, so we'll do the same with our custom funcs
constexpr int INVALID = -1;
enum LuaType {LUA_TNONE = -1, LUA_TNIL = 0, LUA_TBOOLEAN = 1, LUA_TLIGHTUSERDATA = 2, LUA_TNUMBER = 3, LUA_TSTRING = 4, LUA_TTABLE = 5, LUA_TFUNCTION = 6, LUA_TUSERDATA = 7, LUA_TTHREAD = 8};

typedef void HksState;


/*
* Gets the function to be executed's nth param as a string.
*/
char* hksGetParamString(HksState* hksState, int paramIndex)
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
* New funcs and hooks
*/


static intptr_t getBaseFromType(PointerBaseType baseType, void* hksState, intptr_t chrIns)
{
    if (baseType == GAME)
        return getProcessBase();
    if (baseType == CHR_INS)
        return chrIns;

    return 0;
}

int newEnvFunc(void** chrInsPtr, int envId, HksState* hksState)
{
    //env(TRAVERSE_POINTER_CHAIN, pointerBaseType, pointerOffsets...)
    switch (envId) 
    {
    case TRAVERSE_POINTER_CHAIN:
    {
        if (!hksHasParamInt(hksState, 2))
            return INVALID;
        intptr_t chrIns = (intptr_t)*chrInsPtr;
        intptr_t address = getBaseFromType((PointerBaseType)hks_luaL_checkint(hksState, 2), hksState, chrIns);

        int paramIndex = 3;
        while (hksHasParamInt(hksState, paramIndex))
        {
            if (address == 0)
                return INVALID;

            int offset = hks_luaL_checkint(hksState, paramIndex);
            address = *(intptr_t*)(address + offset);
            paramIndex++;
        }

        //get first 32 bits
        return address & 0xFFFFFFFF;
    }

    case EXECUTE_FUNCTION:
    {
        if (!hksHasParamInt(hksState, 2))
            return INVALID;
        void* func = (void*)(hks_luaL_checkint(hksState, 2) + getProcessBase());
        break;
    }

    case GET_EVENT_FLAG:
    {
        if (!hksHasParamInt(hksState, 2))
            return INVALID;

        return getEventFlag(*VirtualMemoryFlag, hks_luaL_checkint(hksState, 2));
        break;
    }

    }

    return 0;
}

static int envHookFunc(void** chrInsPtr, int envId, HksState* hksState)
{
    int newRes = newEnvFunc(chrInsPtr, envId, hksState);
    int originalRes = hksEnv(chrInsPtr, envId, hksState);

    //if original env returns 0 either the envId wasn't vanilla so take the new env, or it was vanilla and just returned 0, at which case newRes would be 0 too anyways.
    if (originalRes != 0)
        return originalRes;
    return newRes;
}

static void newActFunc(void** chrInsPtr, int actId, HksState* hksState) 
{
    switch (actId) 
    {
    case WRITE_POINTER_CHAIN:
    {
        if (!hksHasParamInt(hksState, 2) || !hksHasParamInt(hksState, 3) || !hksHasParamInt(hksState, 4) || !hksHasParamInt(hksState, 5))
            return;
        intptr_t chrIns = (intptr_t)*chrInsPtr;
        intptr_t address = getBaseFromType((PointerBaseType)hks_luaL_checkint(hksState, 2), hksState, chrIns);
        int valType = hks_luaL_checkint(hksState, 3);

        int paramIndex = 5;
        while (hksHasParamInt(hksState, paramIndex + 1))
        {
            if (address == 0)
                return;

            int offset = hks_luaL_checkint(hksState, paramIndex);
            address = *(intptr_t*)(address + offset);
            paramIndex++;
        }
        if (address == 0)
            return;
        intptr_t addrToSet = address + hks_luaL_checkint(hksState, paramIndex);

        //don't even know if the bit shenanigans are necessary for unsigned casts
        switch (valType)
        {
        case SET_UNSIGNED_BYTE:
        {
            int val = hks_luaL_checkint(hksState, 4) & 0xFF;
            *(unsigned char*)addrToSet = *(unsigned char*)(&val);
            break;
        }
        case SET_SIGNED_BYTE:
        {
            *(char*)addrToSet = (char)(hks_luaL_checkint(hksState, 4) & 0xFF);
            break;
        }
        case SET_UNSIGNED_SHORT:
        {
            int val = hks_luaL_checkint(hksState, 4) & 0xFFFF;
            *(unsigned short*)addrToSet = *(unsigned short*)(&val);
            break;
        }
        case SET_SIGNED_SHORT:
        {
            *(short*)addrToSet = (short)(hks_luaL_checkint(hksState, 4) & 0xFFFF);
            break;
        }
        case SET_UNSIGNED_INT:
        {
            *(unsigned int*)addrToSet = (unsigned int)(hks_luaL_checkint(hksState, 4));
            break;
        }
        case SET_SIGNED_INT:
        {
            *(int*)addrToSet = (hks_luaL_checkint(hksState, 4));
            break;
        }
        case SET_FLOAT:
        {
            *(float*)addrToSet = (hks_luaL_checknumber(hksState, 4));
            break;
        }
        }
        break;
    }

    case DEBUG_PRINT:
    {
        if (hks_lua_type(hksState, 2) != LUA_TSTRING)
            return;
        if (GetConsoleWindow() == NULL) 
        {
            AllocConsole();
            freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
            exposerLog("Created Scripts-Data-Exposer-FS Console");
        }
        exposerLog("[HKS Exposer Debug]: " + hksParamToString(hksState, 2));
        break;
    }

    case UPDATE_MAGICID:
    {
        intptr_t chrIns = (intptr_t)*chrInsPtr;
        void* (*getPlayerGameData)(intptr_t) = (void* (*)(intptr_t))(*(intptr_t*)(*(intptr_t*)chrIns + 0x168));
        void* playerGameData = getPlayerGameData(chrIns);
        if (playerGameData == NULL)
            return;
        void* equipData = *(void**)((intptr_t)playerGameData + 0x518);
        if (equipData == NULL)
            return;
        int activeSlot = *(int*)((intptr_t)equipData + 0x80);
        int magicId = *(int*)((intptr_t)equipData + activeSlot * 8 + 0x10);
        intptr_t* magicModule = *(intptr_t**)((*(intptr_t*)(chrIns + 0x190)) + 0x60);
        ((void(*)(intptr_t*, int))(*(intptr_t*)(*magicModule + 0x20)))(magicModule, magicId);
        break;
    }

    case SET_EVENT_FLAG:
    {
        if (!hksHasParamInt(hksState, 2) || !hksHasParamInt(hksState, 3))
            return;

        setEventFlag(*VirtualMemoryFlag, hks_luaL_checkint(hksState, 2), hks_luaL_checkint(hksState, 3));
        break;
    }
    }
}

static void actHookFunc(void** chrInsPtr, int actId, HksState* hksState)
{
    newActFunc(chrInsPtr, actId, hksState);
    hksAct(chrInsPtr, actId, hksState);
}