#pragma once

enum PointerBaseType { GAME = 0, CHR_INS = 1, TARGET_CHR_INS = 2 };

enum ValueInAddressType
{
	UNSIGNED_BYTE_ADDR = 0,
	SIGNED_BYTE_ADDR = 1,
	UNSIGNED_SHORT_ADDR = 2,
	SIGNED_SHORT_ADDR = 3,
	UNSIGNED_INT_ADDR = 4,
	SIGNED_INT_ADDR = 5,
	FLOAT_ADDR = 6,
	BIT_ADDR = 7,
};

typedef void HksState;

enum LuaType { LUA_TNONE = -1, LUA_TNIL = 0, LUA_TBOOLEAN = 1, LUA_TLIGHTUSERDATA = 2, LUA_TNUMBER = 3, LUA_TSTRING = 4, LUA_TTABLE = 5, LUA_TFUNCTION = 6, LUA_TUSERDATA = 7, LUA_TTHREAD = 8 };

//from thefithmatt
struct EventInsId 
{
	int bank;
	int id;
	void* unk_10;
};

union EventArg 
{
	unsigned char asUByte;
	char asByte;
	unsigned short asUShort;
	short asShort;
	unsigned int asUInt;
	int asInt;
	float asFloat;
};

struct CSEmkEventIns
{
	void* vftable;
	void* unk_8;
	uint8_t shortArgs[16];
	int* unkArgData;
	CSEmkEventIns* prevEvent;
	int eventId;
	int unk34;
	int eventId_;
	int unk3C;
	uint8_t unk40[0x30];
	int unk70;
	int unk74;
	int playerId;
	int unk7C;
	uint8_t unk80[0x50];
	// Note: when argData is null, id->unk10 and 0xC8 (and further pointers) are deferenced too.
	// 0xD0
	EventInsId* eventInsId;
	uint8_t* argData;
	uint8_t unkE0[0xD0];
};

typedef wchar_t ModelName[5];

struct ChrSpawnDbgProperties 
{
	wchar_t model[6];
	bool isPlayer = false;

	int npcParam = 0;
	int npcThinkParam = 0;
	int eventEntityId = 0;
	int talkId = 0;
	float posX = 0;
	float posY = 0;
	float posZ = 0;

	int charaInitParam = 0;
	int manipulatorType = 5;
};
