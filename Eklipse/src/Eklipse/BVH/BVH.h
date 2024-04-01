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

            Ref<Node> left;
            Ref<Node> right;
            Vec<Triangle> triangles;
        };

    public:
        BVH() = default;

        void Build(Vec<Triangle>& triangles, const glm::mat4& transform, uint32_t meshIndex);

        Ref<Node> GetRoot();
        AABB& GetAABB();

    private:
        Ref<Node> BuildRecursive(const Vec<Triangle>& triangles, const AABB& parentAABB, uint32_t depth);
        int FindSplitAxis(const Vec<Triangle>& triangles, const AABB& aabb);
        float FindSplitPosition(const Vec<Triangle>& triangles, int axis, const AABB& aabb);

    private:
        Ref<Node> m_root;
        AABB m_aabb;
        uint32_t m_meshIndex = 0;
    };
}