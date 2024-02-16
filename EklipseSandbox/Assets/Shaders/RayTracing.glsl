#stage vertex
#version 460 core

layout(location = 0) in vec2 inPos;

void main() 
{
    gl_Position = vec4(inPos, 0.0, 1.0);
}

#stage fragment
#version 460 core

out vec4 outColor;

layout(push_constant) uniform PushConstants
{
    vec2 uResolution;
} pushConstants;

float rand(vec2 co)
{
    float seed = dot(co, vec2(12.9898, 78.233));
    return fract(sin(seed) * 43758.5453);
}

void main()
{
    vec2 uv = gl_FragCoord.xy / pushConstants.uResolution.xy;
    // float randomValue = rand(uv);
    outColor = vec4(uv, 0.0, 1.0);
}