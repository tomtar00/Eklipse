// layout (std430, binding = 2) buffer TriangleData {
//     Triangle Triangles[];
// } bTriangles;

layout (std430, binding = 2) buffer VertexData {
    float Vertices[];
} bVertices;
layout (std430, binding = 7) buffer TransVertexData {
    float Vertices[];
} bTransVertices;

layout (std430, binding = 3) buffer IndexData {
    uint Indices[];
} bIndices;

layout (std430, binding = 4) buffer MeshData {
    uint NumMeshes;
    MeshInfo Meshes[];
} bMeshes;

layout (std430, binding = 5) buffer MaterialData {
    Material Materials[];
} bMaterials;

layout (std430, binding = 6) buffer TransformData {
    mat4 Transforms[];
} bTransforms;