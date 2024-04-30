#pragma once
#include <string>

namespace Logger 
{
    template<typename ... Args> static inline void log(std::string str, Args ... args)
    {
        std::printf(str.append("\n").c_str(), args ...);
    }
}