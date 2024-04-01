#pragma once
#include "BVH.h"
#include <Eklipse/Renderer/Mesh.h>

namespace Eklipse
{
    class Mesh;
    class Scene;

    class SceneBVH
    {
    public:
        SceneBVH() = default;

        void Build(Scene* scene);

        Vec<BVH::FlatNode>& GetFlatNodes();
        Vec<Triangle> GetTriangles() const;

    private:
        void TraverseRecursive(Ref<BVH::Node> node, uint32_t index);
        Ref<BVH::Node> BuildRecursive(const Vec<Ref<BVH>>& BVHs, const AABB& parentAABB);
        int FindSplitAxis(const Vec<Ref<BVH>>& BVHs, const AABB& aabb);
        float FindSplitPosition(const Vec<Ref<BVH>>& BVHs, int axis, const AABB& aabb);

    private:
        uint32_t m_trianglesCounter;
        Ref<BVH::Node> m_root;
        Vec<BVH::FlatNode> m_flatNodes;
        Vec<Triangle> m_triangles;
    };
}