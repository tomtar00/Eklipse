vec3 GetEnvironmentLight(Ray ray) {
    float skyGradientT = pow(smoothstep(0, 0.4, ray.dir.y), 0.35);
	float groundToSkyT = smoothstep(-0.01, 0, ray.dir.y);
	vec3 skyGradient = mix(pData.SkyColorHorizon, pData.SkyColorZenith, skyGradientT);
	float sun = pow(max(0, dot(ray.dir, normalize(pData.SunDirection))), pData.SunFocus) * pData.SunIntensity;
	vec3 composite = mix(pData.GroundColor, skyGradient, groundToSkyT) + pData.SunColor * sun * float(groundToSkyT>=1);
	return composite;
}
vec3 Trace(Ray ray, inout uint seed) {
    vec3 light = vec3(0.0);
    vec3 color = vec3(1.0);
    for (uint i = 0; i < pData.MaxBounces; i++) {
        HitInfo hitInfo = CalculateRayCollision(ray);
        Material material = hitInfo.material;
        if (hitInfo.didHit) {
            ray.origin = hitInfo.hitPoint;
            vec3 diffuseDir = normalize(hitInfo.normal + randomDirection(seed));
            vec3 specularDir = reflect(ray.dir, hitInfo.normal);
            bool isSpecularBounce = material.specularProb >= nextRandom(seed);
            ray.dir = mix(diffuseDir, specularDir, hitInfo.material.smoothness * float(isSpecularBounce));

            vec3 emittedLight = material.emissionColor * material.emissionStrength;
            light += color * emittedLight * 3.0;
            color *= mix(material.albedo, material.specularColor, float(isSpecularBounce));
        }
        else {
            light += GetEnvironmentLight(ray) * color;
            break;
        }
    }
    return light;
}
vec3 RayTrace(Ray ray, inout uint seed) {
    vec3 totalLight = vec3(0.0);
    for (int i = 0; i < pData.RaysPerPixel; i++) {
        totalLight += Trace(ray, seed);
    }
    return totalLight / float(pData.RaysPerPixel);
}