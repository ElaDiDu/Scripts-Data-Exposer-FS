#pragma once
#include <string>
#include "ProcessData.h"
#include "../include/Logger.h"

enum EnvId 
{
    TRAVERSE_POINTER_CHAIN = 10000,
    EXECUTE_FUNCTION = 10002, 
    GET_EVENT_FLAG = 10003,
    GET_PARAM = 10004,
};
enum ActId 
{ 
    WRITE_POINTER_CHAIN = 10000, 
    DEBUG_PRINT = 10001, 
    UPDATE_MAGICID = 10002, 
    SET_EVENT_FLAG = 10003,
    SET_PARAM = 10004,

    //ESD
    REPLACE_TOOL = 10159,

};


//hks functions return invalid when something is wrong, so we'll do the same with our custom funcs
constexpr int INVALID = -1;

enum LuaType {LUA_TNONE = -1, LUA_TNIL = 0, LUA_TBOOLEAN = 1, LUA_TLIGHTUSERDATA = 2, LUA_TNUMBER = 3, LUA_TSTRING = 4, LUA_TTABLE = 5, LUA_TFUNCTION = 6, LUA_TUSERDATA = 7, LUA_TTHREAD = 8};
static int SINGLE_BIT_MASKS[] = {1, 2, 4, 8, 16, 32, 64, 128};

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
* Helper functions
*/

inline bool isPlayerIns(void* chrIns)
{
    return ((bool (*)(void*))(*(intptr_t*)(*(intptr_t*)chrIns + 0x118)))(chrIns);
}

/// <summary>
/// Starting offset for pointer traversing functions
/// </summary>
/// <param name="baseType"></param>
/// <param name="hksState"></param>
/// <param name="chrIns"></param>
/// <returns></returns>
inline intptr_t getBaseFromType(PointerBaseType baseType, void* hksState, void* chrIns)
{
    if (baseType == GAME)
        return getProcessBase();
    if (baseType == CHR_INS)
        return (intptr_t)chrIns;
    if (baseType == TARGET_CHR_INS) 
    {
        if (isPlayerIns(chrIns))
        {
            //playerIns->targetHandle
            uint64_t targetHandle = *(uint64_t*)((intptr_t)chrIns + 0x6b0);
            return (intptr_t)getChrInsFromHandle(*WorldChrMan, &targetHandle);
        }
    }

    return 0;
}

void* getParamRowEntry(int paramIndex, int rowId)
{
    intptr_t param = (intptr_t)getParamData(paramIndex);
    if (param == NULL) return NULL;

    short rowCount = *(short*)(param + 0xA);
    for (int i = 0; i <= rowCount; i++)
    {
        int currId = *(int*)(param + 0x40 + 0x18 * i);
        if (currId == rowId)
        {
            return (void*)(param + *(int*)(param + 0x48 + 0x18 * i));
        }
        //rows are sorted (I think)
        else if (currId > rowId) return NULL;
    }

    return NULL;
}

int getValueFromAddress(intptr_t address, ValueInAddressType valueType, char bitOffset = 0) 
{
    switch (valueType) 
    {
    case UNSIGNED_BYTE_ADDR:
        return *(unsigned char*)address;
    case SIGNED_BYTE_ADDR:
        return *(char*)address;
    case UNSIGNED_SHORT_ADDR:
        return *(unsigned short*)address;
    case SIGNED_SHORT_ADDR:
        return *(short*)address;
    case UNSIGNED_INT_ADDR:
        return *(unsigned int*)address;
    case SIGNED_INT_ADDR:
        return *(int*)address;
    case FLOAT_ADDR:
        //return the float as int represntation (1.0f = 0x3f800000) because env only returns ints
        return *(unsigned int*)address;
    case BIT_ADDR:
        return ((*(unsigned char*)address) & SINGLE_BIT_MASKS[bitOffset]) != 0;
    }
}

void setValueFromAddress(intptr_t address, ValueInAddressType valueType, float value, char bitOffset = 0)
{
    switch (valueType)
    {
    case UNSIGNED_BYTE_ADDR:
        *(unsigned char*)address = (unsigned char)value;
        return;
    case SIGNED_BYTE_ADDR:
        *(char*)address = (char)value;
        return;
    case UNSIGNED_SHORT_ADDR:
        *(unsigned short*)address = (unsigned short)value;
        return;
    case SIGNED_SHORT_ADDR:
        *(short*)address = (short)value;
        return;
    case UNSIGNED_INT_ADDR:
        *(unsigned int*)address = (unsigned int)value;
        return;
    case SIGNED_INT_ADDR:
        *(int*)address = (int)value;
        return;
    case FLOAT_ADDR:
        *(float*)address = (float)value;
        return;
    case BIT_ADDR:
        if ((char)value == 0) 
            *(uint8_t*)(address) = *(uint8_t*)(address) & ~SINGLE_BIT_MASKS[bitOffset];
        else
            *(uint8_t*)(address) = *(uint8_t*)(address) | SINGLE_BIT_MASKS[bitOffset];
        return;
    }
}

//New hook functions

int newEnvFunc(void** chrInsPtr, int envId, HksState* hksState)
{
    switch (envId) 
    {
    case TRAVERSE_POINTER_CHAIN:
    {
        //pointerBaseType, valueType, bitOffset/pointerOffset1, pointerOffsets...)
        if (!hksHasParamInt(hksState, 2) || !hksHasParamInt(hksState, 3) || !hksHasParamInt(hksState, 4))
            return INVALID;
        intptr_t address = getBaseFromType((PointerBaseType)hks_luaL_checkint(hksState, 2), hksState, *chrInsPtr);
        if (address == 0)
            return INVALID;

        int valueType = hks_luaL_checkint(hksState, 3);
        int paramIndex = 4;
        int bitOffset = 0;

        if (valueType == BIT_ADDR) 
        {
            if (!hksHasParamInt(hksState, 5))
                return INVALID;

            paramIndex = 5;
            bitOffset = hks_luaL_checkint(hksState, 4);
        }
        else
            paramIndex = 4;

        while (hksHasParamInt(hksState, paramIndex + 1))
        {
            if (address == 0)
                return INVALID;

            int offset = hks_luaL_checkint(hksState, paramIndex);
            address = *(intptr_t*)(address + offset);
            paramIndex++;
        }
        address = address + hks_luaL_checkint(hksState, paramIndex);

        return getValueFromAddress(address, (ValueInAddressType)valueType, bitOffset);
    }
    case GET_EVENT_FLAG:
    {
        //flagId
        if (!hksHasParamInt(hksState, 2))
            return INVALID;

        return getEventFlag(*VirtualMemoryFlag, hks_luaL_checkint(hksState, 2));
    }
    case GET_PARAM: 
    {
        //paramIndex, row, offset, valueType, <optional> bitOffset
        if (!hksHasParamInt(hksState, 2) || !hksHasParamInt(hksState, 3) || !hksHasParamInt(hksState, 4) || !hksHasParamInt(hksState, 5))
            return INVALID;

        void* rowEntry = getParamRowEntry(hks_luaL_checkint(hksState, 2), hks_luaL_checkint(hksState, 3));
        if (rowEntry == NULL)
            return INVALID;
        intptr_t valAddr = (intptr_t)rowEntry + hks_luaL_checkint(hksState, 4);


        int valType = hks_luaL_checkint(hksState, 5);
        int bitOffset = 0;

        if (valType == BIT_ADDR)
        {
            if (!hksHasParamInt(hksState, 6)) return INVALID;
            bitOffset = hks_luaL_checkint(hksState, 6);
            if (bitOffset > 7) return INVALID;
        }

        return getValueFromAddress(valAddr, (ValueInAddressType)valType, bitOffset);
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
        //base, valueType, value, bitOffset/pointerOffset1, pointerOffsets...
        if (!hksHasParamInt(hksState, 2) || !hksHasParamInt(hksState, 3) || !hksHasParamInt(hksState, 4) || !hksHasParamInt(hksState, 5))
            return;
        intptr_t address = getBaseFromType((PointerBaseType)hks_luaL_checkint(hksState, 2), hksState, *chrInsPtr);
        int valType = hks_luaL_checkint(hksState, 3);
        int paramIndex = 4;
        int bitOffset = 0;

        if (valType == BIT_ADDR)
        {
            if (!hksHasParamInt(hksState, 6))
                return;

            paramIndex = 6;
            bitOffset = hks_luaL_checkint(hksState, 5);
        }
        else
            paramIndex = 5;

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
        address = address + hks_luaL_checkint(hksState, paramIndex);

        setValueFromAddress(address, (ValueInAddressType)valType, hks_luaL_checknumber(hksState, 4), bitOffset);
        
        break;
    }

    case DEBUG_PRINT:
    {
        //str
        if (hks_lua_type(hksState, 2) != LUA_TSTRING)
            return;
        if (GetConsoleWindow() == NULL) 
        {
            AllocConsole();
            freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
            Logger::log("Created Scripts-Data-Exposer-FS Console");
        }
        Logger::log("[HKS Exposer Debug]: " + hksParamToString(hksState, 2));
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
        //flagId, flagValue
        if (!hksHasParamInt(hksState, 2) || !hksHasParamInt(hksState, 3))
            return;

        setEventFlag(*VirtualMemoryFlag, hks_luaL_checkint(hksState, 2), hks_luaL_checkint(hksState, 3) != 0);
        break;
    }
    case SET_PARAM:
    {
        //paramIndex, row, offset, valueType, value, <optional> bitOffset
        if (!hksHasParamInt(hksState, 2) || !hksHasParamInt(hksState, 3) || !hksHasParamInt(hksState, 4) || !hksHasParamInt(hksState, 5) || !hksHasParamInt(hksState, 6))
            return;

        void* rowEntry = getParamRowEntry(hks_luaL_checkint(hksState, 2), hks_luaL_checkint(hksState, 3));
        if (rowEntry == NULL) 
            return;

        intptr_t addrToSet = (intptr_t)rowEntry + hks_luaL_checkint(hksState, 4);
        int valType = hks_luaL_checkint(hksState, 5);
        int bitOffset = 0;

        if (valType == BIT_ADDR) 
        {
            if (!hksHasParamInt(hksState, 7)) return;
            int bitOffset = hks_luaL_checkint(hksState, 7);
            if (bitOffset > 7) return;
        }

        setValueFromAddress(addrToSet, (ValueInAddressType)valType, hks_luaL_checknumber(hksState, 6), bitOffset);

        break;
    }

    //ESD Functions
    case REPLACE_TOOL:
    {
        if (!hksHasParamInt(hksState, 2) || !hksHasParamInt(hksState, 3))
            return;
        int toReplace = hks_luaL_checkint(hksState, 2);
        int replaceWith = hks_luaL_checkint(hksState, 3);
        char unkChar = hksHasParamInt(hksState, 4) ? hks_luaL_checkint(hksState, 4) : 1;

        replaceItem(toReplace, replaceWith, unkChar);
    }
    }
}

static void actHookFunc(void** chrInsPtr, int actId, HksState* hksState)
{
    newActFunc(chrInsPtr, actId, hksState);
    hksAct(chrInsPtr, actId, hksState);
}