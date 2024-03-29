#pragma once

namespace Eklipse
{
    template <typename T>
    void HashCombine(size_t& seed, const T& value) 
    {
        std::hash<T> hasher;
        seed ^= hasher(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    template <typename T>
    String HashToString(const T& value)
    {
        size_t seed = 0;
        HashCombine(seed, value);
        return std::to_string(seed);
    }

    size_t HashVertex(const Vec<float>& vertex);
}