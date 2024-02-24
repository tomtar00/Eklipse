HitInfo RayTriangle(Ray ray, Triangle triangle)
{
    // vec3 eAB = triangle.b - triangle.a;
    // vec3 eAC = triangle.c - triangle.a;
    // vec3 normal = cross(eAB, eAC);
    // vec3 ao = ray.origin - triangle.a;
    // vec3 dao = cross(ray.dir, ao);

    // float det = -dot(ray.dir, normal);
    // float invDet = 1.0 / det;

    // float dst = dot(ao, normal) * invDet;
    // float u = dot(dao, eAC) * invDet;
    // float v = dot(eAB, dao) * invDet;
    // float w = 1.0 - u - v;

    // HitInfo hitInfo;
    // hitInfo.didHit = dst > 0.0 && u >= 0.0 && v >= 0.0 && w >= 0.0;
    // hitInfo.dst = dst;
    // hitInfo.hitPoint = ray.origin + ray.dir * dst;
    // hitInfo.normal = normalize(triangle.na * w + triangle.nb * u + triangle.nc * v);
    // return hitInfo;

    HitInfo hitInfo;
    hitInfo.didHit = false;

    vec3 edge1 = triangle.b - triangle.a;
    vec3 edge2 = triangle.c - triangle.a;
    vec3 h = cross(ray.dir, edge2);
    float a = dot(edge1, h);

    if (abs(a) < 0.00001) // Ray parallel to triangle
        return hitInfo;

    float f = 1.0 / a;
    vec3 s = ray.origin - triangle.a;
    float u = f * dot(s, h);

    if (u < 0.0 || u > 1.0)
        return hitInfo;

    vec3 q = cross(s, edge1);
    float v = f * dot(ray.dir, q);

    if (v < 0.0 || u + v > 1.0)
        return hitInfo;

    float t = f * dot(edge2, q);
    if (t > 0.00001) {
        hitInfo.didHit = true;
        hitInfo.dst = t;
        hitInfo.hitPoint = ray.origin + ray.dir * t;
        hitInfo.normal = cross(edge1, edge2);
    }

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

HitInfo CalculateRayCollision(Ray ray) {
    HitInfo closestHit;
    closestHit.dst = 1000000.0;
    closestHit.didHit = false;
    closestHit.material = Material(vec3(0.0), 0.0, 0.0, vec3(0.0), vec3(0.0), 0.0);

    for (int i = 0; i < bMeshes.NumMeshes; i++) {
        MeshInfo meshInfo = bMeshes.Meshes[i];
        
        //if (!RayBounds(ray, meshInfo.boundMin, meshInfo.boundMax)) {
        //    continue;
        //}

        for (int j = 0; j < meshInfo.numTriangles; j++) {
            Triangle triangle = bTriangles.Triangles[meshInfo.firstTriangle + j];
            HitInfo hitInfo = RayTriangle(ray, triangle);
            if (hitInfo.didHit && hitInfo.dst < closestHit.dst) {
                closestHit = hitInfo;
                closestHit.material = meshInfo.material;
            }
        }
    }

    return closestHit;
}