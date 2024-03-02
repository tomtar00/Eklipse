#stage vertex
#version 460 core

layout(binding = 0) uniform Camera 
{
	mat4 ViewProjection;
} uCamera;

layout(push_constant) uniform VertexConstants 
{
	mat4 Model;
} uVertConst;

in vec3 inPosition;
in vec3 inNormal;
in vec3 inColor;
in vec2 inTexCoord;

void main() 
{
    gl_Position = uCamera.ViewProjection * uVertConst.Model * vec4(inPosition, 1.0);
}

#stage fragment
#version 460 core

out vec4 outColor;

void main()
{
    outColor = vec4(1.0, 1.0, 1.0, 1.0);
}