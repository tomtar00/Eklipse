struct Triangle
{
    vec3 a;
    vec3 b;
    vec3 c;
};
struct MeshInfo
{
    uint firstTriangle;
    uint numTriangles;
    vec3 boundMin;
    vec3 boundMax;
    Material material;
};