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

    float BVH::SplitCost(const std::vector<AABB>& boxes, int axis, float splitPos)
    {
        AABB leftBounds;
        AABB rightBounds;

        for (const auto& box : boxes)
        {
            if (box.Center()[axis] < splitPos)
            {
                for (int i = 0; i < 3; ++i)
                {
                    leftBounds.Expand(box.GetMin());
                    leftBounds.Expand(box.GetMax());
                }
            }
            else
            {
                for (int i = 0; i < 3; ++i)
                {
                    rightBounds.Expand(box.GetMin());
                    rightBounds.Expand(box.GetMax());
                }
            }
        }

        return leftBounds.SurfaceArea() * boxes.size() + rightBounds.SurfaceArea() * boxes.size();
    }
    int BVH::FindSplitAxis(const Vec<AABB>& boxes)
    {
        EK_CORE_PROFILE();

        float bestCost = FLT_MAX;
        int bestAxis = 0;

        for (int axis = 0; axis < 3; ++axis)
        {
            std::vector<float> centers;
            for (const auto& box : boxes)
            {
                centers.push_back(box.Center()[axis]);
            }
            std::sort(centers.begin(), centers.end());

            for (size_t i = 1; i < centers.size(); ++i)
            {
                float splitPos = (centers[i - 1] + centers[i]) / 2.0f;
                float cost = SplitCost(boxes, axis, splitPos);

                if (cost < bestCost)
                {
                    bestCost = cost;
                    bestAxis = axis;
                }
            }
        }

        return bestAxis;
    }
    float BVH::FindSplitPosition(const Vec<AABB>& boxes, int axis)
    {
        EK_CORE_PROFILE();

        std::vector<float> centerPoints;
        for (const auto& box : boxes)
        {
            centerPoints.push_back(box.Center()[axis]);
        }
        std::sort(centerPoints.begin(), centerPoints.end());
        return centerPoints[centerPoints.size() / 2];
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
            return node;
        }

        int axis = FindSplitAxis(triangles);
        float splitPos = FindSplitPosition(triangles, axis);

        Vec<Triangle> leftTriangles, rightTriangles;
        AABB leftAABB, rightAABB;
        for (const auto& triangle : triangles)
        {
            AABB triangleAABB;
            triangleAABB.Expand(triangle.a);
            triangleAABB.Expand(triangle.b);
            triangleAABB.Expand(triangle.c);

            if (triangleAABB.Center()[axis] < splitPos)
            {
                leftTriangles.push_back(triangle);
                leftAABB.Expand(triangleAABB);
            }
            else
            {
                rightTriangles.push_back(triangle);
                rightAABB.Expand(triangleAABB);
            }
        }

        node->left = BuildRecursive(leftTriangles, leftAABB, depth + 1);
        node->right = BuildRecursive(rightTriangles, rightAABB, depth + 1);
        return node;
    }
    int BVH::FindSplitAxis(const Vec<Triangle>& triangles)
    {
        EK_CORE_PROFILE();

        Vec<AABB> boxes;
        for (const auto& triangle : triangles)
        {
            AABB triangleAABB;
            triangleAABB.Expand(triangle.a);
            triangleAABB.Expand(triangle.b);
            triangleAABB.Expand(triangle.c);
            boxes.push_back(triangleAABB);
        }

        return BVH::FindSplitAxis(boxes);
    }
    float BVH::FindSplitPosition(const Vec<Triangle>& triangles, int axis)
    {
        EK_CORE_PROFILE();

        Vec<AABB> boxes;
        for (const auto& triangle : triangles)
        {
            AABB triangleAABB;
            triangleAABB.Expand(triangle.a);
            triangleAABB.Expand(triangle.b);
            triangleAABB.Expand(triangle.c);
            boxes.push_back(triangleAABB);
        }

        return BVH::FindSplitPosition(boxes, axis);
    }
}