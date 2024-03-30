#pragma once
#include "AABB.h"
#include <Eklipse/Renderer/Mesh.h>

namespace Eklipse
{
    class Mesh;
    class Scene;

    class SceneBVH
    {
    public:
        struct Node
        {
            glm::vec3 min;
            glm::vec3 max;
        };

    public:
        SceneBVH(Scene* scene) : m_scene(scene) {}

        void Build();
        Vec<Node>& GetNodes();

    private:
        void BuildRecursive(const Vec<Triangle>& triangles, uint32_t nodeIndex, const AABB& parentAABB, uint32_t depth);
        int FindSplitAxis(const Vec<Triangle>& triangles, const AABB& aabb);
        float SceneBVH::FindSplitPosition(const Vec<Triangle>& triangles, int axis, const AABB& aabb);

    private: 
        Scene* m_scene;
        Vec<Node> m_nodes;
    };
}