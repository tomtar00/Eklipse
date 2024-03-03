#stage compute
#version 460
#include "common/structs.glsl"
#include "common/mesh/structs.glsl"
#include "common/mesh/uniforms.glsl"

//layout (local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

void main() 
{
    uint index = gl_GlobalInvocationID.x;
    Triangle triangle = bTriangles.Triangles[index];
    triangle.a *= 2.0;
    triangle.b *= 2.0;
    triangle.c *= 2.0;
}