layout (std430, binding = 2) buffer TriangleData {
    Triangle Triangles[];
} bTriangles;

layout (std430, binding = 3) buffer MeshData {
    uint NumMeshes;
    MeshInfo Meshes[];
} bMeshes;