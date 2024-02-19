#stage vertex
#version 460 core

layout(location = 0) in vec2 inPos;

void main() {
    gl_Position = vec4(inPos, 0.0, 1.0);
}

#stage fragment
#version 460 core
#include "common/uniforms.glsl"
#include "common/rand.glsl"
#include "common/sphere_trace.glsl"

out vec4 fragColor;

void main() {
    vec2 uv = gl_FragCoord.xy / pData.Resolution.xy;
    vec4 clipSpacePosition = GetClipSpacePosition(uv);

    Ray ray = GetFragRay(clipSpacePosition);
    fragColor = vec4(RayTrace(ray, uv), 1.0);
}