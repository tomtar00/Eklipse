#version 450

layout(binding = 0) uniform UniformBufferObject 
{
    mat4 mvp;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;

layout(location = 0) out vec3 fragColor;

void main() 
{
    gl_PointSize = 25.0;
    gl_Position = ubo.mvp * vec4(inPosition.xyz, 1.0);
    fragColor = inColor.rgb;
}