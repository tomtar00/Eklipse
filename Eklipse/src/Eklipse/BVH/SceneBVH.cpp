#include "precompiled.h"
#include "SceneBVH.h"

#include <Eklipse/Scene/Components.h>

namespace Eklipse
{
    void SceneBVH::Build(Scene* scene)
    {
        EK_CORE_PROFILE();
        EK_CORE_TRACE("Building BVH...");

        m_trianglesCounter = 0;
        m_root = nullptr;
        m_flatNodes.clear();
        m_triangles.clear();

        AABB sceneAABB;
        Vec<Ref<BVH>> meshBVH;

        scene->GetRegistry().view<RayTracingMeshComponent>().each([&](auto entityID, RayTracingMeshComponent& rtMesh) 
        {
            Entity entity = { entityID, scene };
            auto mesh = entity.GetComponent<MeshComponent>().mesh;
            auto& transformMat = entity.GetComponent<TransformComponent>().GetTransformMatrix();

            Ref<BVH> bvh = CreateRef<BVH>();
            Vec<Triangle> triangles = mesh->GetTriangles();
            bvh->Build(triangles, transformMat, rtMesh.index);
            meshBVH.push_back(bvh);

            AABB meshAABB = bvh->GetAABB();
            sceneAABB.Expand(meshAABB.GetMin());
            sceneAABB.Expand(meshAABB.GetMax());
        });

        if (!meshBVH.empty())
        {
            m_root = BuildRecursive(meshBVH, sceneAABB);
            if (m_root)
            {
                TraverseRecursive(m_root, 0);
            }
        }

        EK_CORE_DBG("BVH built with {} nodes and {} triangles", m_flatNodes.size(), m_triangles.size());
    }
    Vec<BVH::FlatNode>& SceneBVH::GetFlatNodes()
    {
        return m_flatNodes;
    }
    Vec<Triangle> SceneBVH::GetTriangles() const
    {
        return m_triangles;
    }

    void SceneBVH::TraverseRecursive(Ref<BVH::Node> node, uint32_t index)
    {
        EK_CORE_PROFILE();

        BVH::FlatNode flatNode{};
        flatNode.min = node->min;
        flatNode.max = node->max;
        flatNode.isLeaf = node->isLeaf;
        flatNode.meshIndex = node->meshIndex;
        flatNode.startTriIndex = m_trianglesCounter;
        flatNode.endTriIndex = m_trianglesCounter + node->triangles.size();
        flatNode.leftChildIndex = 2 * index + 1;
        flatNode.rightChildIndex = 2 * index + 2;
        m_flatNodes.push_back(flatNode);

        if (node->triangles.size() > 0)
        {
            m_triangles.insert(m_triangles.end(), node->triangles.begin(), node->triangles.end());
            m_trianglesCounter += node->triangles.size();
        }

        if (node->isLeaf)
        {
            return;
        }

        TraverseRecursive(node->left, flatNode.leftChildIndex);
        TraverseRecursive(node->right, flatNode.rightChildIndex);
    }
    Ref<BVH::Node> SceneBVH::BuildRecursive(const Vec<Ref<BVH>>& BVHs, const AABB& parentAABB)
    {
        EK_CORE_PROFILE();

        Ref<BVH::Node> node = CreateRef<BVH::Node>();
        node->min = parentAABB.GetMin();
        node->max = parentAABB.GetMax();
        node->meshIndex = -1;
        node->isLeaf = 0;
        
        if (BVHs.size() == 1)
        {
            return BVHs[0]->GetRoot();
        }

        int axis = FindSplitAxis(BVHs, parentAABB);
        float splitPos = FindSplitPosition(BVHs, axis, parentAABB);

        Vec<Ref<BVH>> leftBVH, rightBVH;
        AABB leftAABB, rightAABB;
        for (const auto& BVH : BVHs)
        {
            if (BVH->GetAABB().Center()[axis] < splitPos)
            {
                leftBVH.push_back(BVH);
                leftAABB.Expand(BVH->GetAABB().GetMin());
                leftAABB.Expand(BVH->GetAABB().GetMax());
            }
            else 
            {
                rightBVH.push_back(BVH);
                rightAABB.Expand(BVH->GetAABB().GetMin());
                rightAABB.Expand(BVH->GetAABB().GetMax());
            }
        }

        node->left = BuildRecursive(leftBVH, leftAABB);
        node->right = BuildRecursive(rightBVH, rightAABB);
        return node;
    }

    static float SplitCost(const std::vector<AABB>& boxes, int axis, float splitPos) 
    {
        AABB leftBounds;
        AABB rightBounds;

        for (const auto& box : boxes) 
        {
            if (box.GetMin()[axis] < splitPos) 
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
    int SceneBVH::FindSplitAxis(const Vec<Ref<BVH>>& BVHs, const AABB& aabb)
    {
        EK_CORE_PROFILE();

        float bestCost = FLT_MAX;
        int bestAxis = 0;

        for (int axis = 0; axis < 3; ++axis) 
        {
            std::vector<float> centers;
            Vec<AABB> boxes;
            for (const auto& bvh : BVHs) 
            {
                boxes.push_back(bvh->GetAABB());
                centers.push_back(bvh->GetAABB().Center()[axis]);
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

    float SceneBVH::FindSplitPosition(const Vec<Ref<BVH>>& BVHs, int axis, const AABB& aabb)
    {
        EK_CORE_PROFILE();

        std::vector<float> centerPoints;
        for (const auto& BVH : BVHs) 
        {
            centerPoints.push_back(BVH->GetAABB().Center()[axis]);
        }

        std::sort(centerPoints.begin(), centerPoints.end());

        int medianIndex = centerPoints.size() / 2;
        return centerPoints[medianIndex];
    }
}
