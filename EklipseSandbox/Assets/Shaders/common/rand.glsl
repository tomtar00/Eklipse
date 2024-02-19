float rand(vec2 co) {
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}
float randNormalDist(vec2 co) {
    float theta = 2.0 * 3.1415926535897932384626433832795 * rand(co);
    float r = sqrt(-2.0 * log(1.0 - rand(co)));
    return r * cos(theta);
}
vec3 randomDirection(vec2 co) {
    float x = randNormalDist(co);
    float y = randNormalDist(co + vec2(0.5));
    float z = randNormalDist(co + vec2(1.0));
    return normalize(vec3(x, y, z));
}
vec3 randomHemisphereDir(vec3 normal, vec2 co) {
    vec3 randDir = randomDirection(co);
    return randDir * sign(dot(normal, randDir));
}