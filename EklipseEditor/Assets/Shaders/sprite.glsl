// Sprite Shader

#stage vertex
#version 430 core

layout (location = 0) in vec2 inPos;
layout (location = 1) in vec2 inTexCoords;

layout(location = 0) out vec2 texCoords;

void main()
{
    gl_Position = vec4(inPos.x, inPos.y, 0.0, 1.0); 
    texCoords = inTexCoords;
}  

#stage fragment
#version 430 core

layout(location = 0) out vec4 FragColor;
layout(location = 0) in vec2 texCoords;

layout(binding = 2) uniform sampler2D screenTexture;

void main()
{
	FragColor = texture(screenTexture, texCoords);
}