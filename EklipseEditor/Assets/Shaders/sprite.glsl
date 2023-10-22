// Sprite Shader

#stage vertex
#version 460 core

in vec2 inPos;
in vec2 inTexCoords;

out vec2 texCoords;

void main()
{
    gl_Position = vec4(inPos.x, inPos.y, 0.0, 1.0); 
    texCoords = inTexCoords;
}  

#stage fragment
#version 460 core

in vec2 texCoords;

out vec4 FragColor;

layout(binding = 2) uniform sampler2D screenTexture;

void main()
{
	FragColor = texture(screenTexture, texCoords);
}