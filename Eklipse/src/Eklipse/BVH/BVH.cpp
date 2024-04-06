#include "precompiled.h"
#include "BVH.h"

namespace Eklipse
{
    BVH::BVH(Vec<Triangle>& triangles, const glm::mat4& transform, uint32_t meshIndex)
    {
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

        m_triangles = triangles;
    }
    Ref<BVH::Node> BVH::Build(uint32_t* startIndex)
    {
        EK_CORE_PROFILE();

        m_indexCounterPtr = startIndex;
        m_root = BuildRecursive(m_triangles, m_aabb, 0);
        return m_root;
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
                leftBounds.Expand(box);
            }
            else
            {
                rightBounds.Expand(box);
            }
        }

        return leftBounds.SurfaceArea() * boxes.size() + rightBounds.SurfaceArea() * boxes.size();
    }
    BVH::SplitResponse BVH::FindSplit(const Vec<AABB>& boxes)
    {
        EK_CORE_PROFILE();

        float bestCost = FLT_MAX;
        SplitResponse response{};

        for (int axis = 0; axis < 3; ++axis)
        {
            std::vector<float> centers;
            for (const auto& box : boxes)
            {
                centers.push_back(box.Center()[axis]);
            }
            std::sort(centers.begin(), centers.end());
            int splitIndex = centers.size() / 2;
            float mainSplitPos = (centers[splitIndex - 1] + centers[splitIndex]) / 2.0f;

        for (size_t i = 1; i < centers.size(); ++i)
            {
                float splitPos = (centers[i - 1] + centers[i]) / 2.0f;
                float cost = SplitCost(boxes, axis, splitPos);

                if (cost < bestCost)
                {
                    bestCost = cost;
                    response.axis = axis;
                    response.splitPos = mainSplitPos;
                }
            }
        }

        return response;
    }

    Ref<BVH::Node> BVH::BuildRecursive(const Vec<Triangle>& triangles, const AABB& parentAABB, uint32_t depth)
    {
        EK_CORE_PROFILE();

        if (triangles.empty())
            return nullptr;

        Ref<BVH::Node> node = CreateRef<BVH::Node>();
        node->min = parentAABB.GetMin();
        node->max = parentAABB.GetMax();
        node->meshIndex = m_meshIndex;
        node->isLeaf = 0;
        node->index = (*m_indexCounterPtr)++;

        if (triangles.size() <= Renderer::GetSettings().maxTrianglesPerLeaf || depth >= Renderer::GetSettings().maxBVHDepth)
        {
            node->isLeaf = 1;
            node->triangles = triangles;
            return node;
        }

        SplitResponse response = FindSplit(triangles);

        Vec<Triangle> leftTriangles, rightTriangles;
        AABB leftAABB, rightAABB;
        for (const auto& triangle : triangles)
        {
            AABB triangleAABB;
            triangleAABB.Expand(triangle.a);
            triangleAABB.Expand(triangle.b);
            triangleAABB.Expand(triangle.c);

            if (triangleAABB.Center()[response.axis] < response.splitPos)
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
    BVH::SplitResponse BVH::FindSplit(const Vec<Triangle>& triangles)
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

        return BVH::FindSplit(boxes);
    }
}