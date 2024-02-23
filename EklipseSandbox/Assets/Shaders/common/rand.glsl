float nextRandom(inout uint seed) {
    seed = seed * 747796405 + 2891336453;
	uint result = ((seed >> ((seed >> 28) + 4)) ^ seed) * 277803737;
	result = (result >> 22) ^ result;
	return result / 4294967295.0;
}
float randNormalDist(inout uint seed) {
    float theta = 2.0 * 3.1415926535897932384626433832795 * nextRandom(seed);
    float r = sqrt(-2.0 * log(nextRandom(seed)));
    return r * cos(theta);
}
vec3 randomDirection(inout uint seed) {
    float x = randNormalDist(seed);
    float y = randNormalDist(seed);
    float z = randNormalDist(seed);
    return normalize(vec3(x, y, z));
}

uint GetRandomSeed(uint pixelIndex) {
    return pixelIndex + pData.Frames * 719393;
}