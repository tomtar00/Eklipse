const int NumSpheres = 3;
Sphere spheres[NumSpheres] = Sphere[](
    Sphere(vec3(-0.6, 0.0, 0.0), 0.5, Material(vec3(1.0, 0.0, 0.0), 0.5, 0.5, vec3(0.0), 0.0)),
    Sphere(vec3(0.6, 0.0, 0.0), 0.5, Material(vec3(0.0, 1.0, 0.0), 0.5, 0.5, vec3(0.0), 0.0)),
    // Sphere(vec3(0, 3, 0.0), 2.0, Material(vec3(0.0, 0.5, 1.0), 0.5, 0.5, vec3(1.0), 3.0)),
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