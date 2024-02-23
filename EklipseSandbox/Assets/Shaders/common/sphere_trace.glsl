const int NumSpheres = 4;
Sphere spheres[NumSpheres] = Sphere[](
    Sphere(vec3(-0.5, 0.0, 0.0), 0.5, Material(vec3(1.0, 0.0, 0.0), 1.0, 0.1, vec3(1.0), vec3(0.0), 0.0)),
    Sphere(vec3(0.5, 0.0, 0.0), 0.5, Material(vec3(0.0, 1.0, 0.0), 0.1, 0.0, vec3(1.0), vec3(0.0), 0.0)),
    Sphere(vec3(0.0, 0.87, 0.0), 0.5, Material(vec3(0.0, 0.0, 1.0), 1.0, 0.9, vec3(1.0), vec3(0.0), 0.0)),

    Sphere(vec3(0.0, -15.5, 0), 15.0, Material(vec3(1.0, 1.0, 1.0), 0.0, 0.0, vec3(1.0), vec3(0.0), 0.0))
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