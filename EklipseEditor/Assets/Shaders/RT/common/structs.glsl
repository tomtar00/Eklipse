struct Ray {
    vec3 origin;
    vec3 dir;
};
struct Material {
    vec3 albedo;
    float smoothness;
    vec3 specularColor;
    float specularProb;
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
    uint materialIndex;
};
struct Triangle {
    vec3 a;
    vec3 b;
    vec3 c;
};
struct MeshInfo {
    uint vertexOffset;
    uint vertexCount;
    uint indexOffset;
    uint indexCount;
    vec3 boundMin;
    vec3 boundMax;
    uint materialIndex;
};
struct BVHNode {
    vec3 boxMin;
    vec3 boxMax;
    vec3 data;
};