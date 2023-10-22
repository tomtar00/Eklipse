// Mesh Shader

#stage vertex
#version 460 core

layout(binding = 0) uniform Camera 
{
	mat4 ViewProjection;
} uCamera;

layout(push_constant) uniform Constants 
{
	mat4 Model;
} pConstants;

in vec3 inPosition;
in vec3 inColor;
in vec2 inTexCoord;

out vec3 fragColor;
out vec2 fragTexCoord;

void main() 
{
    gl_Position = uCamera.ViewProjection * pConstants.Model * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragTexCoord = inTexCoord;
}

#stage fragment
#version 460 core

layout(binding = 1) uniform sampler2D texSampler;

in vec3 fragColor;
in vec2 fragTexCoord;

out vec4 outColor;

void main()
{
    outColor = texture(texSampler, fragTexCoord);
}