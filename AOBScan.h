#pragma once
#include "include/mem/mem.h"
#include "include/mem/pattern.h"
#include "include/mem/simd_scanner.h"
#include "Logger.h"
#include "ProcessData.h"
#include <iomanip>
#include <sstream>

static void* text;
static size_t text_size;


//Regular AOB scan
inline void* AOBScanAddress(const unsigned char* AOBString, const char* AOBMask, const void* region = text, const size_t region_size = text_size)
{
    const auto pattern = mem::pattern(AOBString, AOBMask);
    const auto mregion = mem::region(region, region_size);
    char* result = mem::simd_scanner(pattern).scan(mregion).any();

    if (!result)
    {
        size_t AOBStrLen = std::strlen(AOBMask);
        std::stringstream str;
        str << "AOB string ";
        for (size_t i = 0; i < AOBStrLen; i++)
        {
            str << std::hex << static_cast<int>(AOBString[i]) << " ";
        }
        str << "not found.";
        exposerLog(str.str());
    }

    return reinterpret_cast<void*>(result);
}

//AOB scan with offset (e.g for when the AOB is inside the function)
inline void* AOBScanCode(const uint8_t* AOBString, const char* AOBMask, const int Offset = 0, const void* region = text, const size_t region_size = text_size)
{
    uint8_t* addr = reinterpret_cast<uint8_t*>(AOBScanAddress(AOBString, AOBMask, region, region_size));

    if (!addr)
    {
        return nullptr;
    }

    return reinterpret_cast<void*>(addr + Offset);
}

//AOB scan for a global pointer [base] through an instruction that uses it
inline void** AOBScanBase(const unsigned char* AOBString, const char* AOBMask, const int InOffset = 7, const int OpOffset = 3) 
{
    uint8_t* addr = static_cast<uint8_t*>(AOBScanAddress(AOBString, AOBMask));
    return addr != nullptr ? reinterpret_cast<void**>(addr + *reinterpret_cast<int32_t*>(addr + OpOffset) + InOffset) : nullptr;
}

inline void* AOBScanFuncCall(const unsigned char* AOBString1, const char* AOBMask1, const unsigned char* AOBString2, const char* AOBMask2, const size_t FuncSize, const int CallOffset)
{
    uint8_t* addr = reinterpret_cast<uint8_t*>(AOBScanAddress(AOBString1, AOBMask1));

    if (!addr)
    {
        return nullptr;
    }

    addr = addr + *reinterpret_cast<int32_t*>(addr + 1 + CallOffset) + 5 + CallOffset;
    return AOBScanAddress(AOBString2, AOBMask2, addr, FuncSize);
}

//AOB scan for a function through a different function that calls it
inline void* AOBScanCodeCall(const unsigned char* AOBString, const char* AOBMask, const int FuncStartToOpOffset, const int OpOffset)
{
    uint8_t* addr = static_cast<uint8_t*>(AOBScanAddress(AOBString, AOBMask));
    return addr != nullptr ? reinterpret_cast<void*>(addr + *reinterpret_cast<int32_t*>(addr + OpOffset + 1) + 5 + FuncStartToOpOffset) : nullptr;
}

extern void GetText()
{
    const char textStrMatch[] = ".text"; // refused to work with char* string

    const auto pattern = mem::pattern(textStrMatch, ".....");
    const auto region = mem::region(PROCESS_INFO.mInfo.lpBaseOfDll, PROCESS_INFO.mInfo.SizeOfImage);
    uint8_t* result = mem::simd_scanner(pattern).scan(region).any();

    text_size = *reinterpret_cast<uint32_t*>(result + 0x10);
    text = result + *reinterpret_cast<uint32_t*>(result + 0x14);
};