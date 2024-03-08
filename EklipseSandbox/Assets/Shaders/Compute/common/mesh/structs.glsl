struct Triangle
{
    vec3 a;
    vec3 b;
    vec3 c;
};
struct MeshInfo
{
    uint vertexOffset;
    uint vertexCount;
    uint indexOffset;
    uint indexCount;
    uint materialIndex;
    vec3 boundMin;
    vec3 boundMax;
};