HitInfo RayTriangle(Ray ray, Triangle triangle)
{
    vec3 eAB = triangle.b - triangle.a;
    vec3 eAC = triangle.c - triangle.a;
    vec3 normal = cross(eAB, eAC);
    vec3 ao = ray.origin - triangle.a;
    vec3 dao = cross(ray.dir, ao);

    float det = -dot(ray.dir, normal);
    float invDet = 1.0 / det;

    float dst = dot(ao, normal) * invDet;
    float u = dot(dao, eAC) * invDet;
    float v = dot(eAB, dao) * invDet;
    float w = 1.0 - u - v;

    HitInfo hitInfo;
    hitInfo.didHit = dst > 0.0 && u >= 0.0 && v >= 0.0 && w >= 0.0;
    hitInfo.dst = dst;
    hitInfo.hitPoint = ray.origin + ray.dir * dst;
    hitInfo.normal = normalize(triangle.na * w + triangle.nb * u + triangle.nc * v);
    return hitInfo;
}
bool RayBounds(Ray ray, vec3 boxMin, vec3 boxMax)
{
	vec3 invDir = 1 / ray.dir;
	vec3 tMin = (boxMin - ray.origin) * invDir;
	vec3 tMax = (boxMax - ray.origin) * invDir;
	vec3 t1 = min(tMin, tMax);
	vec3 t2 = max(tMin, tMax);
	float tNear = max(max(t1.x, t1.y), t1.z);
	float tFar = min(min(t2.x, t2.y), t2.z);
	return tNear <= tFar;
};