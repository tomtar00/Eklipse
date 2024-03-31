#include "precompiled.h"
#include "BVH.h"

namespace Eklipse
{
    void BVH::Build(Vec<Triangle>& triangles, const glm::mat4& transform, uint32_t meshIndex)
    {
        EK_CORE_PROFILE();

        m_meshIndex = meshIndex;
        m_aabb = AABB();
        for (auto& triangle : triangles) 
        {
            triangle.a = glm::vec3(transform * glm::vec4(triangle.a, 1.0f));
            triangle.b = glm::vec3(transform * glm::vec4(triangle.b, 1.0f));
            triangle.c = glm::vec3(transform * glm::vec4(triangle.c, 1.0f));

            m_aabb.Expand(triangle.a);
            m_aabb.Expand(triangle.b);
            m_aabb.Expand(triangle.c);
        }

        m_numTriangles = 0;
        m_root = BuildRecursive(triangles, m_aabb, 0);
    }
    Ref<BVH::Node> BVH::GetRoot()
    {
        return m_root;
    }
    AABB& BVH::GetAABB()
    {
        return m_aabb;
    }

    Ref<BVH::Node> BVH::BuildRecursive(const Vec<Triangle>& triangles, const AABB& parentAABB, uint32_t depth)
    {
        EK_CORE_PROFILE();

        Ref<BVH::Node> node = CreateRef<BVH::Node>();
        node->min = parentAABB.GetMin();
        node->max = parentAABB.GetMax();
        node->meshIndex = m_meshIndex;
        node->isLeaf = 0;

        if (triangles.size() <= Renderer::GetSettings().maxTrianglesPerLeaf || depth >= Renderer::GetSettings().maxBVHDepth)
        {
            node->isLeaf = 1;
            node->triangles = triangles;
            node->startTriIndex = m_numTriangles;
            m_numTriangles += triangles.size();
            node->endTriIndex = m_numTriangles;
            return node;
        }

        int axis = FindSplitAxis(triangles, parentAABB);
        float splitPos = FindSplitPosition(triangles, axis, parentAABB);

        Vec<Triangle> leftTriangles, rightTriangles;
        AABB leftAABB, rightAABB;
        for (const auto& triangle : triangles)
        {
            AABB triangleAABB;
            triangleAABB.Expand(triangle.a);
            triangleAABB.Expand(triangle.b);
            triangleAABB.Expand(triangle.c);
            if (triangleAABB.GetMax()[axis] < splitPos)
            {
                leftTriangles.push_back(triangle);
                leftAABB.Expand(triangle.a);
                leftAABB.Expand(triangle.b);
                leftAABB.Expand(triangle.c);
            }
            else
            {
                rightTriangles.push_back(triangle);
                rightAABB.Expand(triangle.a);
                rightAABB.Expand(triangle.b);
                rightAABB.Expand(triangle.c);
            }
        }

        node->left = BuildRecursive(leftTriangles, leftAABB, depth + 1);
        node->right = BuildRecursive(rightTriangles, rightAABB, depth + 1);
        return node;
    }
    int BVH::FindSplitAxis(const Vec<Triangle>& triangles, const AABB& aabb)
    {
        EK_CORE_PROFILE();

        return aabb.MaxDim();
    }
    float BVH::FindSplitPosition(const Vec<Triangle>& triangles, int axis, const AABB& aabb)
    {
        EK_CORE_PROFILE();

        std::vector<float> centerPoints;
        for (const auto& triangle : triangles) {
            centerPoints.push_back((triangle.a[axis] + triangle.b[axis] + triangle.c[axis]) / 3.0f);
        }
        std::sort(centerPoints.begin(), centerPoints.end());
        return centerPoints[centerPoints.size() / 2];
    }
}