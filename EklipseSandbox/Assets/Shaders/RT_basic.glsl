#stage vertex
#version 460 core

layout(location = 0) in vec2 inPos;

void main() {
    gl_Position = vec4(inPos, 0.0, 1.0);
}

#stage fragment
#version 460 core
#include "common/structs.glsl"
#include "common/uniforms.glsl"
#include "common/rand.glsl"
#include "common/sphere_trace.glsl"
#include "common/mesh_trace.glsl"
#include "common/trace.glsl"
#include "common/util.glsl"

out vec4 fragColor;

void main() {
    vec2 uv = gl_FragCoord.xy / pData.Resolution.xy;
    vec4 clipSpacePosition = GetClipSpacePosition(uv);
    uint pixelIndex = uint(gl_FragCoord.x + gl_FragCoord.y * pData.Resolution.x);
    uint randomSeed = GetRandomSeed(pixelIndex);

    Ray ray = GetFragRay(clipSpacePosition);
    fragColor = vec4(RayTrace(ray, randomSeed), 1.0);
}