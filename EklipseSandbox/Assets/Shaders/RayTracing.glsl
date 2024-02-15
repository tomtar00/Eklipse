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

float rand(vec2 co)
{
    float seed = dot(co, vec2(12.9898, 78.233));
    return fract(sin(seed) * 43758.5453);
}

void main()
{
    vec2 uv = gl_FragCoord.xy;
    float randomValue = rand(uv);
    outColor = vec4(randomValue, randomValue, 0.0, 1.0);
}