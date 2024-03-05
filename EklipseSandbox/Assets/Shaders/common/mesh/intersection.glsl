HitInfo RayTriangle(Ray ray, Triangle triangle)
{
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
        
        // if (!RayBounds(ray, meshInfo.boundMin, meshInfo.boundMax)) {
        //    continue;
        // }

        for (uint j = meshInfo.indexOffset; j < meshInfo.indexCount; j += 3) {
            Triangle triangle;
            uint idx1 = bIndices.Indices[j];
            uint idx2 = bIndices.Indices[j + 1];
            uint idx3 = bIndices.Indices[j + 2];
            triangle.a = vec3(bVertices.Vertices[idx1 * 3], bVertices.Vertices[idx1 * 3 + 1], bVertices.Vertices[idx1 * 3 + 2]);
            triangle.b = vec3(bVertices.Vertices[idx2 * 3], bVertices.Vertices[idx2 * 3 + 1], bVertices.Vertices[idx2 * 3 + 2]);
            triangle.c = vec3(bVertices.Vertices[idx3 * 3], bVertices.Vertices[idx3 * 3 + 1], bVertices.Vertices[idx3 * 3 + 2]);

            HitInfo hitInfo = RayTriangle(ray, triangle);
            if (hitInfo.didHit && hitInfo.dst < closestHit.dst) {
                closestHit = hitInfo;
                closestHit.material = bMaterials.Materials[meshInfo.materialIndex];
            }
        }
    }

    return closestHit;
}