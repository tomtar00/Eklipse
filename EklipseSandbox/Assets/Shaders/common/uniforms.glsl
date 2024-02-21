layout(binding = 0) uniform Camera {
	mat4 ViewProjection;
} uCamera;

layout (binding = 1) buffer PixelData {
    vec3 Data[];
} bPixels;

layout (binding = 2) buffer MeshData {
    Triangle Triangle[1000];
    MeshInfo Info[100];
    uint NumMeshes;
} bMeshes;

layout(push_constant) uniform Data {

    // 32 bytes
    vec2 Resolution;
    vec3 CameraPos;
    uint Frames;
    uint RaysPerPixel;
    uint MaxBounces;

    // 68 bytes
    vec3 SkyColorHorizon;
    vec3 SkyColorZenith;
    vec3 GroundColor;
    vec3 SunColor;
    vec3 SunDirection;
    float SunFocus;
    float SunIntensity;

} pData;