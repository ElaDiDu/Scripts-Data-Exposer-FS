#pragma once
#include <string>

namespace Logger 
{
    static inline void log(std::string str)
    {
        std::printf(str.append("\n").c_str());
    }
}