#pragma once
#include <string>


static inline void exposerLog(std::string str)
{
    std::printf(str.append("\n").c_str());
}