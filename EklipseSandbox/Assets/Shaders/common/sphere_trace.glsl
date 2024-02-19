struct Ray {
    vec3 origin;
    vec3 dir;
};
struct Material {
    vec3 albedo;
    float roughness;
    float metallic;
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

const int NumSpheres = 4;
Sphere spheres[NumSpheres] = Sphere[](
    Sphere(vec3(-0.6, 0.0, 0.0), 0.5, Material(vec3(1.0, 0.0, 0.0), 0.5, 0.5, vec3(0.0), 0.0)),
    Sphere(vec3(0.6, 0.0, 0.0), 0.5, Material(vec3(0.0, 1.0, 0.0), 0.5, 0.5, vec3(0.0), 0.0)),
    Sphere(vec3(0, 3, 0.0), 2.0, Material(vec3(0.0, 0.5, 1.0), 0.5, 0.5, vec3(1.0), 3.0)),
    Sphere(vec3(0.0, -7.5, -0.5), 7.0, Material(vec3(1.0, 1.0, 1.0), 0.5, 0.5, vec3(0.0), 0.0))
);

HitInfo RaySphere(Ray ray, Sphere sphere) {
    HitInfo hitInfo;
    hitInfo.didHit = false;
    vec3 offsetRayOrigin = ray.origin - sphere.position;
    float a = dot(ray.dir, ray.dir);
    float b = 2 * dot(offsetRayOrigin, ray.dir);
    float c = dot(offsetRayOrigin, offsetRayOrigin) - sphere.radius * sphere.radius;
    float discriminant = b * b - 4 * a * c;
    if (discriminant >= 0) {
        float dst = (-b - sqrt(discriminant)) / (2 * a);
        if (dst >= 0) {
            hitInfo.didHit = true;
            hitInfo.dst = dst;
            hitInfo.hitPoint = ray.origin + ray.dir * dst;
            hitInfo.normal = normalize(hitInfo.hitPoint - sphere.position);
        }
    }
    return hitInfo;
}
HitInfo CalculateRayCollision(Ray ray) {
    HitInfo closestHit;
    closestHit.dst = 1000000.0;
    closestHit.didHit = false;
    closestHit.material = Material(vec3(0.0), 0.0, 0.0, vec3(0.0), 0.0);
    for (int i = 0; i < NumSpheres; i ++) {
		Sphere sphere = spheres[i];
		HitInfo hitInfo = RaySphere(ray, sphere);
		if (hitInfo.didHit && hitInfo.dst < closestHit.dst) {
			closestHit = hitInfo;
			closestHit.material = sphere.material;
		}
	}
    return closestHit;
}
vec3 GetAmbientLight(Ray ray) {
    float skyGradientTime = pow(smoothstep(0.0, 0.4, ray.dir.y), 0.35);
    float groundToSkyTime = smoothstep(-0.01, 0, ray.dir.y);
	vec3 skyGradient = mix(pData.SkyColorHorizon, pData.SkyColorZenith, skyGradientTime);
	float sun = pow(max(0, dot(ray.dir, -normalize(pData.SunDirection))), pData.SunFocus) * pData.SunIntensity;
	return mix(pData.GroundColor, skyGradient, groundToSkyTime) + sun * float(groundToSkyTime >= 1);
}
vec3 Trace(Ray ray, vec2 co) {
    vec3 light = vec3(0.0);
    vec3 color = vec3(1.0);
    for (uint i = 0; i < pData.MaxBounces; i++) {
        HitInfo hitInfo = CalculateRayCollision(ray);
        if (hitInfo.didHit) {
            ray.origin = hitInfo.hitPoint;
            ray.dir = normalize(hitInfo.normal + randomDirection(co));

            Material material = hitInfo.material;
            vec3 emittedLight = material.emissionColor * material.emissionStrength;
            light += color * emittedLight;
            color *= material.albedo;
        }
        else {
            light += GetAmbientLight(ray) * color;
            break;
        }
    }
    return light;
}
vec4 GetClipSpacePosition(vec2 uv) {
    vec4 clipSpacePosition = vec4(uv * 2.0 - 1.0, 1.0, 1.0);
    return clipSpacePosition;
}
Ray GetFragRay(vec4 clipSpacePosition) {
    vec4 viewDir = inverse(uCamera.ViewProjection) * clipSpacePosition;
    viewDir.xyz /= viewDir.w;

    Ray ray;
    ray.origin = pData.CameraPos;
    ray.dir = normalize(viewDir.xyz - ray.origin);
    return ray;
}
vec3 RayTrace(Ray ray, vec2 co) {
    vec3 totalLight = vec3(0.0);
    for (int i = 0; i < pData.RaysPerPixel; i++) {
        totalLight += Trace(ray, co * pData.Frames * (i+1));
    }
    return totalLight / float(pData.RaysPerPixel);
}