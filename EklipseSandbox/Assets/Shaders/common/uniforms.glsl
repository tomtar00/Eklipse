layout(binding = 0) uniform Camera 
{
	mat4 ViewProjection;
} uCamera;
layout (binding = 1) buffer PixelData {
    vec3 Data[];
} bPixels;

layout(push_constant) uniform Data {

    // 36 bytes
    vec2 Resolution;
    vec3 CameraPos;
    uint Frames;
    uint RaysPerPixel;
    uint MaxBounces;
    uint Reset;

    // 56 bytes
    vec3 SkyColorHorizon;
    vec3 SkyColorZenith;
    vec3 GroundColor;
    vec3 SunDirection;
    float SunFocus;
    float SunIntensity;

} pData;