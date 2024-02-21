struct Ray {
    vec3 origin;
    vec3 dir;
};
struct Material {
    vec3 albedo;
    float smoothness;
    float specularProb;
    vec3 specularColor;
    vec3 emissionColor;
    float emissionStrength;
};
struct HitInfo {
    bool didHit;
    float dst;
    vec3 hitPoint;
    vec3 normal;
    Material material;
};
struct Sphere {
    vec3 position;
    float radius;
    Material material;
};
struct Triangle
{
    vec3 a, na;
    vec3 b, nb;
    vec3 c, nc;
};
struct MeshInfo
{
    uint firstTriangle;
    uint numTriangles;
    vec3 boundMin;
    vec3 boundMax;
    Material material;
};