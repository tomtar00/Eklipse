#include "precompiled.h"
#include "Hash.h"

size_t Eklipse::HashVertex(const Vec<float>& vertex)
{
    size_t hash = 0;
    std::hash<float> hasher;
    for (int i = 0; i < vertex.size(); i++)
        hash ^= hasher(vertex[i]) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    return hash;
}