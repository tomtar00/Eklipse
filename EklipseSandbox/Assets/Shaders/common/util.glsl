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