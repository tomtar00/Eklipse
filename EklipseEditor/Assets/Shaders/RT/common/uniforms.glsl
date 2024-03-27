layout(push_constant) uniform Data {

    vec2 Resolution;
    vec3 CameraPos;
    uint Frames;
    uint RaysPerPixel;
    uint MaxBounces;
    uint Accumulate;

    vec3 SkyColorHorizon;
    vec3 SkyColorZenith;
    vec3 GroundColor;
    vec3 SunColor;
    vec3 SunDirection;
    float SunFocus;
    float SunIntensity;

} pData;

layout(binding = 0) uniform Camera {
	mat4 ViewProjection;
} uCamera;

layout (std140, binding = 1) buffer PixelData {
    vec3 Data[];
} bPixels;

layout (std430, binding = 2) buffer VertexData {
    float Vertices[];
} bVertices;
layout (std430, binding = 3) buffer TransVertexData {
    float Vertices[];
} bTransVertices;
layout (std430, binding = 4) buffer IndexData {
    uint Indices[];
} bIndices;

layout (std140, binding = 5) buffer SphereData {
    uint NumSpheres;
    Sphere Spheres[];
} bSpheres;
layout (std140, binding = 6) buffer MeshData {
    uint NumMeshes;
    MeshInfo Meshes[];
} bMeshes;

layout (std140, binding = 7) buffer MaterialData {
    Material Materials[];
} bMaterials;

layout (std140, binding = 8) buffer TransformData {
    mat4 Transforms[];
} bTransforms;

layout (std140, binding = 9) buffer BVHData {
    int TopLevelIndex;
    BVHNode Nodes[];
} bBVH;