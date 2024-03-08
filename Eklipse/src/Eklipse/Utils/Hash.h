#pragma once

namespace Eklipse
{
    template <typename T>
    void HashCombine(size_t& seed, const T& value) 
    {
        seed = std::hash<T>()(value) + (seed << 5) + seed;
    }

    template <typename T>
    String HashToString(const T& value)
    {
        size_t seed = 0;
        HashCombine(seed, value);
        return std::to_string(seed);
    }
}