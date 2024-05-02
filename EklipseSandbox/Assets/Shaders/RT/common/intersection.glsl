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
HitInfo RayTriangle(Ray ray, Triangle triangle) {
    HitInfo hitInfo;
    hitInfo.didHit = false;

    vec3 edge1 = triangle.b - triangle.a;
    vec3 edge2 = triangle.c - triangle.a;
    vec3 h = cross(ray.dir, edge2);
    float a = dot(edge1, h);

    if (abs(a) < 0.00001)
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
        hitInfo.normal = normalize(cross(edge1, edge2));
    }

    return hitInfo;
}
float RayBox(Ray ray, vec3 boxMin, vec3 boxMax) {
    vec3 invDir = 1 / ray.dir;
	vec3 tMin = (boxMin - ray.origin) * invDir;
	vec3 tMax = (boxMax - ray.origin) * invDir;
	vec3 t1 = min(tMin, tMax);
	vec3 t2 = max(tMin, tMax);
	float tFar = min(min(t2.x, t2.y), t2.z);
	float tNear = max(max(t1.x, t1.y), t1.z);
    return (tFar >= tNear) ? (tNear > 0.0 ? tNear : tFar) : -1.0;
}

HitInfo CalculateRayCollision(Ray ray) {
    HitInfo closestHit;
    closestHit.dst = 1000000.0;
    closestHit.didHit = false;
    closestHit.material = Material(vec3(1.0), 0.0, vec3(0.0), 0.0, vec3(0.0), 0.0);

    for (int i = 0; i < bSpheres.NumSpheres; i++) {
		Sphere sphere = bSpheres.Spheres[i];
		HitInfo hitInfo = RaySphere(ray, sphere);
		if (hitInfo.didHit && hitInfo.dst < closestHit.dst) {
			closestHit = hitInfo;
			closestHit.material = bMaterials.Materials[sphere.materialIndex];
		}
	}

    if (pData.UseBVH != 0) {
        int stack[64];
        int stackPtr = 0;
        stack[stackPtr++] = -1;

        BVHNode currentNode = bBVH.Nodes[0];
        MeshInfo meshInfo;
        Triangle triangle;
        HitInfo hitInfo;

        int currentNodeIndex = 0;
        // bool didHitAnyTriangle = false;
        float dstLeft; 
        float dstRight;

        if (RayBox(ray, currentNode.boxMin, currentNode.boxMax) > 0.0) 
        {
            while(currentNodeIndex != -1)
            {
                currentNode = bBVH.Nodes[currentNodeIndex];

                if (currentNode.isLeaf != 0) 
                {
                    meshInfo = bMeshes.Meshes[currentNode.meshIndex];
                    for (uint i = currentNode.startTriIndex; i < currentNode.endTriIndex; ++i) 
                    {
                        triangle = bTriangles.Triangles[i];
                        hitInfo = RayTriangle(ray, triangle);
                        if (hitInfo.didHit && hitInfo.dst < closestHit.dst) 
                        {
                            // didHitAnyTriangle = true;
                            closestHit = hitInfo;
                            closestHit.material = bMaterials.Materials[meshInfo.materialIndex];
                        }
                    }

                    // if (didHitAnyTriangle)
                    // {
                    //     didHitAnyTriangle = false;
                    //     stack[stackPtr++] = -1;
                    // }
                    currentNodeIndex = stack[--stackPtr];
                }
                else
                {
                    if (currentNode.leftChildIndex > 0)
                    {
                        BVHNode left = bBVH.Nodes[currentNode.leftChildIndex];
                        dstLeft = RayBox(ray, left.boxMin, left.boxMax);
                    }
                    else
                    {
                        dstLeft = -1.0;
                    }

                    if (currentNode.rightChildIndex > 0)
                    {
                        BVHNode right = bBVH.Nodes[currentNode.rightChildIndex];
                        dstRight = RayBox(ray, right.boxMin, right.boxMax);
                    }
                    else
                    {
                        dstRight = -1.0;
                    }

                    if (dstLeft > 0.0 && dstRight > 0.0)
                    {
                        if (dstLeft < dstRight)
                        {
                            stack[stackPtr++] = currentNode.rightChildIndex;
                            currentNodeIndex = currentNode.leftChildIndex;
                        }
                        else
                        {
                            stack[stackPtr++] = currentNode.leftChildIndex;
                            currentNodeIndex = currentNode.rightChildIndex;
                        }
                    }
                    else if (dstLeft > 0.0)
                    {
                        currentNodeIndex = currentNode.leftChildIndex;
                    }
                    else if (dstRight > 0.0)
                    {
                        currentNodeIndex = currentNode.rightChildIndex;
                    }
                    else
                    {
                        currentNodeIndex = stack[--stackPtr];
                    }
                }
            }
        }
    }
    else {
        for (int i = 0; i < bMeshes.NumMeshes; i++) {
            MeshInfo meshInfo = bMeshes.Meshes[i];

            for (uint j = meshInfo.indexOffset; j < meshInfo.indexOffset + meshInfo.indexCount; j += 3) {
                Triangle triangle;

                uint idx1 = bIndices.Indices[j + 0] * 3 + meshInfo.vertexOffset;
                uint idx2 = bIndices.Indices[j + 1] * 3 + meshInfo.vertexOffset;
                uint idx3 = bIndices.Indices[j + 2] * 3 + meshInfo.vertexOffset;

                triangle.a = vec3(bTransVertices.Vertices[idx1 + 0], bTransVertices.Vertices[idx1 + 1], bTransVertices.Vertices[idx1 + 2]);
                triangle.b = vec3(bTransVertices.Vertices[idx2 + 0], bTransVertices.Vertices[idx2 + 1], bTransVertices.Vertices[idx2 + 2]);
                triangle.c = vec3(bTransVertices.Vertices[idx3 + 0], bTransVertices.Vertices[idx3 + 1], bTransVertices.Vertices[idx3 + 2]);
         
                HitInfo hitInfo = RayTriangle(ray, triangle);
                if (hitInfo.didHit && hitInfo.dst < closestHit.dst) {
                    closestHit = hitInfo;
                    closestHit.material = bMaterials.Materials[meshInfo.materialIndex];
                }
            }
        }
    }

    return closestHit;
}