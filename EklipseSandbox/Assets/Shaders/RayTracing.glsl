#stage vertex
#version 460 core

layout(location = 0) in vec2 inPos;

void main() {
    gl_Position = vec4(inPos, 0.0, 1.0);
}

#stage fragment
#version 460 core

layout(binding = 0) uniform Camera 
{
	mat4 ViewProjection;
} uCamera;

out vec4 fragColor;

layout(push_constant) uniform PushConstants {
    // vec3 uCameraPos;
    vec2 uResolution;
} pushConstants;

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
    Sphere(vec3(-0.5, 0.0, 0.0), 0.5, Material(vec3(1.0, 0.0, 0.0), 0.5, 0.5, vec3(0.0), 0.0)),
    Sphere(vec3(0.5, 0.0, 0.0), 0.5, Material(vec3(0.0, 1.0, 0.0), 0.5, 0.5, vec3(0.0), 0.0)),
    Sphere(vec3(0, 7, 0.0), 5.0, Material(vec3(0.0, 0.5, 1.0), 0.5, 0.5, vec3(1.0), 1.0)),
    Sphere(vec3(0.0, -7.5, -0.5), 7.0, Material(vec3(1.0, 1.0, 1.0), 0.5, 0.5, vec3(0.0), 0.0))
);

HitInfo RaySphere(Ray ray, vec3 sphereCentre, float sphereRadius) {
    HitInfo hitInfo;
    hitInfo.didHit = false;
    vec3 offsetRayOrigin = ray.origin - sphereCentre;
    float a = dot(ray.dir, ray.dir);
    float b = 2 * dot(offsetRayOrigin, ray.dir);
    float c = dot(offsetRayOrigin, offsetRayOrigin) - sphereRadius * sphereRadius;
    float discriminant = b * b - 4 * a * c;
    if (discriminant >= 0) {
        float dst = (-b - sqrt(discriminant)) / (2 * a);
        if (dst >= 0) {
            hitInfo.didHit = true;
            hitInfo.dst = dst;
            hitInfo.hitPoint = ray.origin + ray.dir * dst;
            hitInfo.normal = normalize(hitInfo.hitPoint - sphereCentre);
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
		HitInfo hitInfo = RaySphere(ray, sphere.position, sphere.radius);
		if (hitInfo.didHit && hitInfo.dst < closestHit.dst) {
			closestHit = hitInfo;
			closestHit.material = sphere.material;
		}
	}
    return closestHit;
}

vec3 Trace(Ray ray, int maxBounces, vec2 co) {
    vec3 light = vec3(0.0);
    vec3 color = vec3(1.0);
    for (int i = 0; i < maxBounces; i++) {
        HitInfo hitInfo = CalculateRayCollision(ray);
        if (hitInfo.didHit) {
            ray.origin = hitInfo.hitPoint;
            ray.dir = normalize(hitInfo.normal + randomDirection(co));
            vec3 emittedLight = hitInfo.material.emissionColor * hitInfo.material.emissionStrength;
            light += color * emittedLight;
            color *= hitInfo.material.albedo;
        }
        else {
            break;
        }
    }
    return light;
}

const int NumRaysPerPixel = 10;
void main() {
    vec2 uv = gl_FragCoord.xy / pushConstants.uResolution.xy;
    uv = uv * 2.0 - 1.0;
    vec4 clipSpacePosition = vec4(uv, 1.0, 1.0);
    vec4 viewDir = inverse(uCamera.ViewProjection) * clipSpacePosition;
    viewDir.xyz /= viewDir.w;

    Ray ray;
    ray.origin = vec3(0.0, 2.0, 10.0);
    ray.dir = normalize(viewDir.xyz - ray.origin);

    vec3 totalLight = vec3(0.0);
    for (int i = 0; i < NumRaysPerPixel; i++) {
        totalLight += Trace(ray, 3, uv);
    }
    fragColor = vec4(totalLight / NumRaysPerPixel, 1.0);
}