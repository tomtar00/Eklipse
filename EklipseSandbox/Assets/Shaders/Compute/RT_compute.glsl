#stage compute
#version 460
#include "common/structs.glsl"
#include "common/uniforms.glsl"

void main() {
    uint meshIndex = 0;
    uint triangleIndex = gl_GlobalInvocationID.x;
    for (uint i = 0; i < bMeshes.NumMeshes; i++) {
        if (triangleIndex >= (bMeshes.Meshes[i].vertexOffset / 3) && triangleIndex < (bMeshes.Meshes[i].vertexOffset + bMeshes.Meshes[i].vertexCount) / 3) {
            meshIndex = i;
            break;
        }
    }
    uint index = gl_GlobalInvocationID.x * 3; 
    vec3 vertex = vec3(bVertices.Vertices[index + 0], bVertices.Vertices[index + 1], bVertices.Vertices[index + 2]);
    mat4 transform = bTransforms.Transforms[meshIndex];
    vec4 transVertex = transform * vec4(vertex, 1.0);
    bTransVertices.Vertices[index + 0] = transVertex.x;
    bTransVertices.Vertices[index + 1] = transVertex.y;
    bTransVertices.Vertices[index + 2] = transVertex.z;
}