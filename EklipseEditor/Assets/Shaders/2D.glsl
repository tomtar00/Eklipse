#stage vertex
#version 460 core

in vec2 inPos;

void main()
{
    gl_Position = vec4(inPos.x, inPos.y, 0.0, 1.0); 
}  

#stage fragment
#version 460 core

out vec4 outColor;

void main()
{
	outColor = vec4(1.0, 1.0, 1.0, 1.0);
}