#stage compute
#version 460
#include "common/structs.glsl"
#include "common/uniforms.glsl"

void main() {
    uint meshIndex = gl_GlobalInvocationID.x;
    mat4 transform = bTransforms.Transforms[meshIndex];
    MeshInfo meshInfo = bMeshes.Meshes[meshIndex];
    vec4 boundsMin = transform * vec4(meshInfo.boundMin, 1.0);
    vec4 boundsMax = transform * vec4(meshInfo.boundMax, 1.0);
    bBounds.Bounds[meshIndex] = Bounds(boundsMin.xyz, boundsMax.xyz);
}