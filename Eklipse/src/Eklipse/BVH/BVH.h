#pragma once
#include "AABB.h"
#include <Eklipse/Renderer/Mesh.h>

namespace Eklipse
{
    class BVH
    {
    public:
        struct FlatNode
        {
            glm::vec3 min;
            int isLeaf;

            glm::vec3 max;
            int meshIndex;

            int leftChildIndex;
            int rightChildIndex;
            int startTriIndex;
            int endTriIndex;
        };
        struct Node
        {
            glm::vec3 min;
            glm::vec3 max;

            int isLeaf;
            int meshIndex;
            int index;

            Ref<Node> left;
            Ref<Node> right;
            Vec<Triangle> triangles;
        };
        struct SplitResponse
        {
            int axis;
            float splitPos;
        };

    public:
        BVH(Vec<Triangle>& triangles, const glm::mat4& transform, uint32_t meshIndex);

        Ref<BVH::Node> Build(uint32_t* startIndex);

        Ref<BVH::Node> GetRoot();
        AABB& GetAABB();

    public:
        static float SplitCost(const std::vector<AABB>& boxes, int axis, float splitPos);
        static SplitResponse FindSplit(const Vec<AABB>& boxes);

    private:
        Ref<BVH::Node> BuildRecursive(const Vec<Triangle>& triangles, const AABB& parentAABB, uint32_t depth);
        SplitResponse FindSplit(const Vec<Triangle>& triangles);

    private:
        Ref<BVH::Node> m_root;
        AABB m_aabb;
        uint32_t m_meshIndex = 0;
        uint32_t* m_indexCounterPtr = 0;
        Vec<Triangle> m_triangles;
    };
}