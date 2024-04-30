#pragma once
#include <string>
#include "ProcessData.h"
#include "../include/Logger.h"

//Given up on emevd for now, conditions and control flow are a big bother and functions aren't nice either


//Unfortunately the emevd system forces me to do this in the least sane way.
enum EventId 
{
	WRITE_POINTER_CHAIN_GAME_BYTE = 1000,
	WRITE_POINTER_CHAIN_GAME_SHORT = 1001,
	WRITE_POINTER_CHAIN_GAME_INT = 1002,
	WRITE_POINTER_CHAIN_GAME_FLOAT = 1003,
	WRITE_POINTER_CHAIN_CHR_BYTE = 1010,
	WRITE_POINTER_CHAIN_CHR_SHORT = 1011,
	WRITE_POINTER_CHAIN_CHR_INT = 1012,
	WRITE_POINTER_CHAIN_CHR_FLOAT = 1013,
};

inline int argAsInt(uint8_t* args, intptr_t* pos)
{
	//round to a pos to a multiple of 4
	if (*pos % 4 != 0) 
	{
		*pos += 4 - (*pos % 4);
	}
	int ret = *(int*)(args + *pos);
	*pos += 4;
	return ret;
}

inline char argAsByte(uint8_t* args, intptr_t* pos)
{
	char ret = *(char*)(args + *pos);
	*pos += 1;
	return ret;
}

inline void comparePointerChainGame(uint8_t* args, size_t argCount) 
{
	intptr_t argPos = 0;
	char comparisonType = argAsByte(args, &argPos);
}

inline bool writePointerChainGame(uint8_t* args, size_t argCount, char valType) 
{
	if (argCount < 2)
		return false;

	//first arg is value to write
	void* valueAddr = args;
	intptr_t argPos = 4;

	intptr_t addr = getProcessBase();
	for (int i = 1; i < argCount - 1; i++) 
	{
		addr = *(intptr_t*)(addr + argAsInt(args, &argPos));
	}
	int finalOffset = argAsInt(args, &argPos);

	switch(valType) 
	{
	case UNSIGNED_BYTE_ADDR:
		*(unsigned char*)(addr + finalOffset) = *(unsigned char*)(valueAddr);
		break;
	case SIGNED_BYTE_ADDR:
		*(char*)(addr + finalOffset) = *(char*)(valueAddr);
		break;
	case UNSIGNED_SHORT_ADDR:
		*(unsigned short*)(addr + finalOffset) = *(unsigned short*)(valueAddr);
		break;
	case SIGNED_SHORT_ADDR:
		*(short*)(addr + finalOffset) = *(short*)(valueAddr);
		break;
	case UNSIGNED_INT_ADDR:
		*(unsigned int*)(addr + finalOffset) = *(unsigned int*)(valueAddr);
		break;
	case SIGNED_INT_ADDR:
		*(int*)(addr + finalOffset) = *(int*)(valueAddr);
		break;
	case FLOAT_ADDR:
		*(float*)(addr + finalOffset) = *(float*)(valueAddr);
		break;
	}

	return true;
}

bool newEmevdSystemFunction(void* unk1, void* unk2, CSEmkEventIns* event) 
{
	switch (event->eventInsId->id) 
	{
	case WRITE_POINTER_CHAIN_GAME_BYTE:
	{
		intptr_t base = getProcessBase();
		uint8_t* args = event->argData;
		if (args == NULL) 
		{
			return false;
		}
		return writePointerChainGame(args, 4, SIGNED_BYTE_ADDR);
	}
	}

	return false;
}