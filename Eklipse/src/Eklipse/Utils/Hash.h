#pragma once

namespace Eklipse
{
    template <typename T>
    void HashCombine(size_t& seed, const T& value) 
    {
        return std::hash<T>()(value) + (seed << 5) + seed;
    }
}