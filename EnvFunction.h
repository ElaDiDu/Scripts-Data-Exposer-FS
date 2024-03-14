#pragma once
#include <vector>

enum ArgDataType {INT, FLOAT, STRING, TABLE, USERDATA};
enum EnvId { TRAVERSE_POINTER_CHAIN = 10000, FP_SET_TEST = 9999 };


class EnvFunction
{
public:
	int envId;
	std::vector<ArgDataType> args;
	void* func;

	EnvFunction(int envId, void* func, std::vector<ArgDataType> args = {}) : envId(envId), func(func), args(args)
	{
	}

	/*int doEnvFunc(void* hksState, void** chrInsPointer) 
	{
		//Lua begins from arg 1, and arg 1 is envId. So we begin from arg 2
		for (int i = 2; i < args.size() + 2; i++)
		{
			if (hksHasParam(hksState, i))
		}
	}*/

	static std::vector<EnvFunction*> getEnvFuncList() 
	{
		return envList;
	}

private:
	static inline std::vector<EnvFunction*> envList;
};

//+ 0x140B910
/*
* Tests if function to be executed has an nth param.
*/
static bool* hksHasParam(bool* out, void* hksState, int paramIndex);

static int hksHasParam(void* hksState, int paramIndex)
{
	bool result = false;
	hksHasParam(&result, hksState, paramIndex);
	return result;
}

//+ 0x14A32C0
/*
* Gets the function to be executed's nth param as an int.
*/
static int hksGetParamInt(void* hksState, int paramIndex);

//+ 0x14A3370
/*
* Gets the function to be executed's nth param as a float.
*/
static int hksGetParamFloat(void* hksState, int paramIndex);

