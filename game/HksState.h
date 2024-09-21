#pragma once
#include <string>
#include "ProcessData.h"
#include "../include/Logger.h"
#include "../include/PointerChain.h"

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
    CHR_SPAWN_DEBUG = 10005,

    //ESD
    REPLACE_TOOL = 100059,

};

//TODO Change all ugly pointer traversals with PointerChain


//hks functions return invalid when something is wrong, so we'll do the same with our custom funcs
constexpr float INVALID = -1;

static int SINGLE_BIT_MASKS[] = { 1, 2, 4, 8, 16, 32, 64, 128 };


/*
* Helper functions
*/




/*
* Convert model id to name
* Examples:
* 0 = c0000
* 420 = c0420
* 4700 = c4700
*/
#define MODEL_LENGTH (5)
inline wchar_t digitToWChar(char digit)
{
    return digit + 0x30;
}

inline bool modelIdToName(int id, wchar_t* name)
{
    if (id < 0 || id > 9999) return false;

    name[0] = L'c';

    for (int i = 1; i < MODEL_LENGTH; i++)
    {
        char digit = id % 10;
        name[MODEL_LENGTH - i] = digitToWChar(digit);
        id = id / 10;
    }

    return true;
}

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

float getValueFromAddress(intptr_t address, int valueType, char bitOffset = 0)
{
    switch (valueType)
    {
    case UNSIGNED_BYTE_ADDR:
        return (float)*(unsigned char*)address;
    case SIGNED_BYTE_ADDR:
        return (float)*(char*)address;
    case UNSIGNED_SHORT_ADDR:
        return (float)*(unsigned short*)address;
    case SIGNED_SHORT_ADDR:
        return (float)*(short*)address;
    case UNSIGNED_INT_ADDR:
        return (float)*(unsigned int*)address;
    case SIGNED_INT_ADDR:
        return (float)*(int*)address;
    case FLOAT_ADDR:
        return (float)*(float*)address;
    case BIT_ADDR:
        return (float)(((*(unsigned char*)address) & SINGLE_BIT_MASKS[bitOffset]) != 0);
    }

    return (float)*(int*)address;
}

void setValueInAddress(intptr_t address, int valueType, int iValue, float fValue, char bitOffset = 0)
{
    switch (valueType)
    {
    case UNSIGNED_BYTE_ADDR:
        *(unsigned char*)address = (unsigned char)iValue;
        return;
    case SIGNED_BYTE_ADDR:
        *(char*)address = (char)iValue;
        return;
    case UNSIGNED_SHORT_ADDR:
        *(unsigned short*)address = (unsigned short)iValue;
        return;
    case SIGNED_SHORT_ADDR:
        *(short*)address = (short)iValue;
        return;
    case UNSIGNED_INT_ADDR:
        *(unsigned int*)address = (unsigned int)iValue;
        return;
    case SIGNED_INT_ADDR:
        *(int*)address = iValue;
        return;
    case FLOAT_ADDR:
        *(float*)address = fValue;
        return;
    case BIT_ADDR:
        if ((char)iValue == 0)
            *(uint8_t*)(address) = *(uint8_t*)(address) & ~SINGLE_BIT_MASKS[bitOffset];
        else
            *(uint8_t*)(address) = *(uint8_t*)(address) | SINGLE_BIT_MASKS[bitOffset];
        return;
    }
}

//New hook functions

auto NO_ACT = "No act.";
auto OK = "OK.";
auto INCORRECT_ARGS = "Not enough arguments.";
auto NULL_POINTER = "Encountered null pointer.";
auto PARAM_DOESNT_EXIST = "Param doesn't exist.";
auto INCORRECT_MODEL = "Incorrect model format.";

/// <summary>
/// Function for new envs
/// </summary>
/// <param name="chrInsPtr"></param>
/// <param name="envId"></param>
/// <param name="hksState"></param>
/// <returns></returns>
std::pair<const char*, float> newEnvFunc(void** chrInsPtr, int envId, HksState* hksState)
{
    switch (envId)
    {
    case TRAVERSE_POINTER_CHAIN:
    {
        //pointerBaseType, valueType, bitOffset/pointerOffset1, pointerOffsets...)
        if (!hksHasParamNumber(hksState, 2) || !hksHasParamNumber(hksState, 3) || !hksHasParamNumber(hksState, 4)) 
        {
            return std::pair(INCORRECT_ARGS, INVALID);
        }
        intptr_t address = getBaseFromType((PointerBaseType)hksGetParamInt(hksState, 2), hksState, *chrInsPtr);
        if (address == 0)
            return std::pair(NULL_POINTER, INVALID);

        int valueType = hksGetParamInt(hksState, 3);
        int paramIndex = 4;
        int bitOffset = 0;

        if (valueType == BIT_ADDR)
        {
            if (!hksHasParamNumber(hksState, 5))
                return std::pair(INCORRECT_ARGS, INVALID);

            paramIndex = 5;
            bitOffset = hksGetParamInt(hksState, 4);
        }
        else
            paramIndex = 4;

        while (hksHasParamNumber(hksState, paramIndex + 1))
        {
            if (address == 0)
                return std::pair(NULL_POINTER, INVALID);

            intptr_t offset = hksGetParamLong(hksState, paramIndex);
    

            address = *(intptr_t*)(address + offset);
            paramIndex++;
        }
        if (address == 0)
            return std::pair(NULL_POINTER, INVALID);

        address = address + hksGetParamLong(hksState, paramIndex);

        return std::pair(OK, getValueFromAddress(address, valueType, bitOffset));
    }
    case GET_EVENT_FLAG:
    {
        //flagId
        if (!hksHasParamNumber(hksState, 2))
            return std::pair(INCORRECT_ARGS, INVALID);

        return std::pair(OK, getEventFlag(*VirtualMemoryFlag, hksGetParamInt(hksState, 2)));
    }
    case GET_PARAM:
    {
        //paramIndex, row, offset, valueType, <optional> bitOffset
        if (!hksHasParamNumber(hksState, 2) || !hksHasParamNumber(hksState, 3) || !hksHasParamNumber(hksState, 4) || !hksHasParamNumber(hksState, 5))
            return std::pair(INCORRECT_ARGS, INVALID);

        void* rowEntry = getParamRowEntry(hksGetParamInt(hksState, 2), hksGetParamInt(hksState, 3));
        if (rowEntry == NULL)
            return std::pair(PARAM_DOESNT_EXIST, INVALID);
        intptr_t valAddr = (intptr_t)rowEntry + hksGetParamInt(hksState, 4);


        int valType = hksGetParamInt(hksState, 5);
        int bitOffset = 0;

        if (valType == BIT_ADDR)
        {
            if (!hksHasParamNumber(hksState, 6)) 
                return std::pair(INCORRECT_ARGS, INVALID);
            bitOffset = hksGetParamInt(hksState, 6);
            if (bitOffset > 7) return 
                std::pair(INCORRECT_ARGS, INVALID);
        }

        return std::pair(OK, getValueFromAddress(valAddr, valType, bitOffset));
    }

    }

    return std::pair(NO_ACT, INVALID);
}

/// <summary>
/// Function for new acts
/// </summary>
/// <param name="chrInsPtr"></param>
/// <param name="actId"></param>
/// <param name="hksState"></param>
/// <returns></returns>
static const char* newActFunc(void** chrInsPtr, int actId, HksState* hksState)
{
    switch (actId)
    {
    case WRITE_POINTER_CHAIN:
    {
        //base, valueType, value, bitOffset/pointerOffset1, pointerOffsets...
        if (!hksHasParamNumber(hksState, 2) || !hksHasParamNumber(hksState, 3) || !hksHasParamNumber(hksState, 4) || !hksHasParamNumber(hksState, 5))
            return INCORRECT_ARGS;
        intptr_t address = getBaseFromType((PointerBaseType)hksGetParamInt(hksState, 2), hksState, *chrInsPtr);
        int valType = hksGetParamInt(hksState, 3);
        int paramIndex = 4;
        int bitOffset = 0;

        if (valType == BIT_ADDR)
        {
            if (!hksHasParamNumber(hksState, 6))
                return INCORRECT_ARGS;

            paramIndex = 6;
            bitOffset = hksGetParamInt(hksState, 5);
        }
        else
            paramIndex = 5;

        while (hksHasParamNumber(hksState, paramIndex + 1))
        {
            if (address == 0)
                return NULL_POINTER;

            intptr_t offset = hksGetParamLong(hksState, paramIndex);
            address = *(intptr_t*)(address + offset);
            paramIndex++;
        }
        if (address == 0)
            return NULL_POINTER;
        address = address + hksGetParamLong(hksState, paramIndex);

        setValueInAddress(address, valType, hksGetParamInt(hksState, 4), hks_luaL_checknumber(hksState, 4), bitOffset);
        return OK;
    }

    case DEBUG_PRINT:
    {
        if (GetConsoleWindow() == NULL)
        {
            AllocConsole();
            freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
            Logger::log("Created Scripts-Data-Exposer-FS Console");
        }
        Logger::log("[HKS Exposer]: " + hksParamToString(hksState, 2));
        return OK;
    }

    case UPDATE_MAGICID:
    {
        void* chrIns = *chrInsPtr;
        void* (*getPlayerGameData)(void*) = *PointerChain::make<void* (*)(void*)>(chrIns, 0x0, 0x168);
        void* playerGameData = getPlayerGameData(chrIns);
        if (playerGameData == NULL)
            return NULL_POINTER;
        void* equipData = *PointerChain::make<void*>(playerGameData, 0x530);
        if (equipData == NULL)
            return NULL_POINTER;
        int activeSlot = *PointerChain::make<int>(equipData, 0x80);;
        int magicId = *(int*)((intptr_t)equipData + activeSlot * 8 + 0x10);
        void* magicModule = *PointerChain::make<void*>(chrIns, 0x190, 0x60);
        void (*updateMagicId)(void*, int) = *PointerChain::make<void (*)(void*, int)>(magicModule, 0x0, 0x20);

        updateMagicId(magicModule, magicId);
        return OK;
    }

    case SET_EVENT_FLAG:
    {
        //flagId, flagValue
        if (!hksHasParamNumber(hksState, 2) || !hksHasParamNumber(hksState, 3))
            return INCORRECT_ARGS;

        setEventFlag(*VirtualMemoryFlag, hksGetParamInt(hksState, 2), hksGetParamInt(hksState, 3) != 0);
        return OK;
    }
    case SET_PARAM:
    {
        //paramIndex, row, offset, valueType, value, <optional> bitOffset
        if (!hksHasParamNumber(hksState, 2) || !hksHasParamNumber(hksState, 3) || !hksHasParamNumber(hksState, 4) || !hksHasParamNumber(hksState, 5) || !hksHasParamNumber(hksState, 6))
            return INCORRECT_ARGS;

        void* rowEntry = getParamRowEntry(hksGetParamInt(hksState, 2), hksGetParamInt(hksState, 3));
        if (rowEntry == NULL)
            return PARAM_DOESNT_EXIST;

        intptr_t addrToSet = (intptr_t)rowEntry + hksGetParamInt(hksState, 4);
        int valType = hksGetParamInt(hksState, 5);
        int bitOffset = 0;

        if (valType == BIT_ADDR)
        {
            if (!hksHasParamNumber(hksState, 7)) 
                return INCORRECT_ARGS;
            bitOffset = hksGetParamInt(hksState, 7);
            if (bitOffset > 7) 
                return INCORRECT_ARGS;
        }

        setValueInAddress(addrToSet, valType, hksGetParamInt(hksState, 6), hks_luaL_checknumber(hksState, 6), bitOffset);
        return OK;
    }
    //TODO Use function instead of just writing to memory so you can make multiple chrs at once. Unfinished for now.
    case CHR_SPAWN_DEBUG:
    {
        //model, npcParam, npcThinkParam, posX, posY, posZ, eventEntityId, talkId, charaInitParam
        if (!hksHasParamNumber(hksState, 2) || !hksHasParamNumber(hksState, 3) || !hksHasParamNumber(hksState, 4) || !hksHasParamNumber(hksState, 5) || !hksHasParamNumber(hksState, 6) || !hksHasParamNumber(hksState, 7))
            return INCORRECT_ARGS;

        ChrSpawnDbgProperties chrProperties;

        int chrId = hksGetParamInt(hksState, 2);
        wchar_t model[6] = { 0 };
        if (!modelIdToName(chrId, model)) //invalid id
            return INCORRECT_MODEL;
        if (chrId == 0)
            chrProperties.isPlayer = true;
        memcpy_s(chrProperties.model, 10, model, 10);

        chrProperties.npcParam = hksGetParamInt(hksState, 3);
        chrProperties.npcThinkParam = hksGetParamInt(hksState, 4);

        chrProperties.posX = hks_luaL_checknumber(hksState, 5);
        chrProperties.posY = hks_luaL_checknumber(hksState, 6);
        chrProperties.posZ = hks_luaL_checknumber(hksState, 7);

        if (hksHasParamNumber(hksState, 8))
            chrProperties.eventEntityId = hksGetParamInt(hksState, 8);
        else
            chrProperties.eventEntityId = 0;

        if (hksHasParamNumber(hksState, 9))
            chrProperties.talkId = hksGetParamInt(hksState, 9);
        else
            chrProperties.talkId = 0;

        if (hksHasParamNumber(hksState, 10))
            chrProperties.charaInitParam = hksGetParamInt(hksState, 10);
        else
            chrProperties.charaInitParam = 0;

        createChrDebug(chrProperties);
        return OK;
    }

    //ESD Functions
    case REPLACE_TOOL:
    {
        if (!hksHasParamNumber(hksState, 2) || !hksHasParamNumber(hksState, 3))
            return INCORRECT_ARGS;
        int toReplace = hksGetParamInt(hksState, 2);
        int replaceWith = hksGetParamInt(hksState, 3);
        char unkChar = hksHasParamNumber(hksState, 4) ? hksGetParamInt(hksState, 4) : 1;

        intptr_t chrIns = (intptr_t)*chrInsPtr;
        if (!isPlayerIns((void*)chrIns)) 
            return OK;

        intptr_t gameData = ((intptr_t(*)(intptr_t))(*(intptr_t*)(*(intptr_t*)chrIns + 0x168)))(chrIns);
        if (gameData == NULL) 
            return NULL_POINTER;
        //gameData->equipData

        replaceItem((void*)(gameData + 0x2b0), toReplace, replaceWith, unkChar);
    }
    }

    return NO_ACT;
}

/// <summary>
/// The actual lua env cfunction pushed onto the lua state.
/// </summary>
/// <param name="hksState"></param>
/// <returns>number of values returned</returns>
static int LuaHks_env(HksState* hksState)
{
    void* chrIns = getHksChrInsOwner(hksState);
    if (chrIns == NULL || !hksHasParamNumber(hksState, 1))
    {
        //ChrIns should never be null, there should always be envId (1st argument)
        hks_lua_pushnumber(hksState, 0);
        return 1;
    }

    //chrIns->chrModules->behaviorScript->chrEnvRunsOn
    //always self?
    void** envTarget = *PointerChain::make<void**>(chrIns, 0x190, 0x10, 0x18);
    int envId = hks_luaL_checkint(hksState, 1);

    //The function acceptable "envId" are mutually exclusive. The one not used must return 0, so their sum would be the result of the used function
    float numRes = 0;
    auto result = newEnvFunc(envTarget, envId, hksState); 
    if (result.first == OK) 
    {
        hks_lua_pushnumber(hksState, result.second);
        return 1;
    }
    else if (result.first == NO_ACT) 
    {
        hks_lua_pushnumber(hksState, (float)hksEnv(envTarget, envId, hksState));
        return 1;
    }
    else
    {
        hksPushNil(hksState);
        hksPushString(hksState, result.first);
        return 2;
    }
    
    return 0;
}

/// <summary>
/// The actual lua act cfunction pushed onto the lua state.
/// </summary>
/// <param name="hksState"></param>
/// <returns>number of values returned</returns>
static int LuaHks_act(HksState* hksState)
{
    void* chrIns = getHksChrInsOwner(hksState);
    if (chrIns == NULL || !hksHasParamNumber(hksState, 1))
    {
        //ChrIns should never be null, there should always be actId (1st argument)
        hks_lua_pushnumber(hksState, 0);
        return 1;
    }

    //chrIns->chrModules->behaviorScript->chrActRunsOn
    //always self?
    void** actTarget = *PointerChain::make<void**>(chrIns, 0x190, 0x10, 0x10);
    int actId = hks_luaL_checkint(hksState, 1);

    //The original act function does seemingly intentionally return numbers, ours doesn't (use new env instead if you can).

    const char* result = newActFunc(actTarget, actId, hksState);
    if (result == OK)
    {
        hks_lua_pushnumber(hksState, 0);
        return 1;
    }
    else if (result == NO_ACT) 
    {
        hks_lua_pushnumber(hksState, (float)hksAct(actTarget, actId, hksState));
        return 1;
    }
    else 
    {
        hksPushNil(hksState);
        hksPushString(hksState, result);
        return 2;
    }

    return 0;
}


static void newPushEnvActGlobalsFunc(HksState* hksState)
{
    hks_addnamedcclosure(hksState, "env", LuaHks_env);
    hks_addnamedcclosure(hksState, "act", LuaHks_act);
}

static void hksSetCGlobalsHookFunc(HksState* hksState)
{
    hksSetCGlobals(hksState);
    newPushEnvActGlobalsFunc(hksState);
}