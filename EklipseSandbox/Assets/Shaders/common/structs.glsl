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