#stage vertex
#version 460 core

layout(location = 0) in vec2 inPos;

void main() {
    gl_Position = vec4(inPos, 0.0, 1.0);
}

#stage fragment
#version 460 core
#include "common/structs.glsl"
#include "common/sphere/structs.glsl"
#include "common/uniforms.glsl"
#include "common/rand.glsl"
#include "common/sphere/intersection.glsl"
#include "common/trace.glsl"
#include "common/util.glsl"

out vec4 fragColor;

void main() {
    vec2 uv = gl_FragCoord.xy / pData.Resolution.xy;
    vec4 clipSpacePosition = GetClipSpacePosition(uv);
    uint pixelIndex = uint(gl_FragCoord.x + gl_FragCoord.y * pData.Resolution.x);
    uint randomSeed = GetRandomSeed(pixelIndex);

    Ray ray = GetFragRay(clipSpacePosition);
    vec3 currentColor = RayTrace(ray, randomSeed);

    float weight = 1.0 / float(pData.Frames);
    vec3 newColor = currentColor * weight + bPixels.Data[pixelIndex] * (1.0 - weight);

    bPixels.Data[pixelIndex] = newColor;
    fragColor = vec4(newColor, 1.0);
}