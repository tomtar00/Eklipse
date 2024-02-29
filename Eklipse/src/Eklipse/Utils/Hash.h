#pragma once

namespace Eklipse
{
    template <typename T>
    void HashCombine(size_t& seed, const T& value) 
    {
        seed = std::hash<T>()(value) + (seed << 5) + seed;
    }
}