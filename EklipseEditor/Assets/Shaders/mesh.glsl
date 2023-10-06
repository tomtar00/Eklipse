// Mesh Shader

#stage vertex
#version 460 core

layout(std140, binding = 0) uniform Camera
{
	mat4 ViewProjection;
} Camera;
layout(std140, binding = 1) uniform Transform
{
	mat4 Model;
} Transform;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main()
{
    gl_Position = Camera.ViewProjection * Transform.Model * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragTexCoord = inTexCoord;
}

#stage fragment
#version 460 core

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = texture(texSampler, fragTexCoord);
}