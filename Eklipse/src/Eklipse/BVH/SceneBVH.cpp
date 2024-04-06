#include "precompiled.h"
#include "SceneBVH.h"

#include <Eklipse/Scene/Components.h>
#include <queue>

namespace Eklipse
{
    void SceneBVH::Build(Scene* scene)
    {
        EK_CORE_PROFILE();
        EK_CORE_TRACE("Building BVH...");

        m_trianglesCounter = 0;
        m_indexCounter = 0;
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

            Vec<Triangle> triangles = mesh->GetTriangles();
            Ref<BVH> bvh = CreateRef<BVH>(triangles, transformMat, rtMesh.index);
            meshBVH.push_back(bvh);

            sceneAABB.Expand(bvh->GetAABB());
        });

        if (!meshBVH.empty())
        {
            auto& min = sceneAABB.GetMin();
            auto& max = sceneAABB.GetMax();
            EK_CORE_TRACE("Scene AABB: min: x={} y={} z={}, max: x={} y={} z={}", min.x, min.y, min.z, max.x, max.y, max.z);
            m_root = BuildRecursive(meshBVH, sceneAABB);
            if (m_root)
            {
                FlattenLevels();
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

    void SceneBVH::FlattenLevels()
    {
        if (m_root == nullptr)
            return;

        Traverse(m_root);
    }
    void SceneBVH::Traverse(Ref<BVH::Node> node)
    {
        BVH::FlatNode flatNode{};
        flatNode.min = node->min;
        flatNode.max = node->max;
        flatNode.isLeaf = node->isLeaf;
        flatNode.meshIndex = node->meshIndex;
        flatNode.startTriIndex = m_trianglesCounter;
        flatNode.endTriIndex = m_trianglesCounter + node->triangles.size();
        flatNode.leftChildIndex = node->left ? node->left->index : -1;
        flatNode.rightChildIndex = node->right ? node->right->index : -1;
        m_flatNodes.push_back(flatNode);

        if (node->triangles.size() > 0)
        {
            m_triangles.insert(m_triangles.end(), node->triangles.begin(), node->triangles.end());
            m_trianglesCounter += node->triangles.size();
        }

        EK_CORE_INFO("FlatNode: index: {}, isLeaf: {}, meshIndex: {}, startTriIndex: {}, endTriIndex: {}, leftChildIndex: {}, rightChildIndex: {}",
            node->index, flatNode.isLeaf, flatNode.meshIndex, flatNode.startTriIndex, flatNode.endTriIndex, flatNode.leftChildIndex, flatNode.rightChildIndex);

        if (node->left)
            Traverse(node->left);
        if (node->right)
            Traverse(node->right);
    }
    Ref<BVH::Node> SceneBVH::BuildRecursive(const Vec<Ref<BVH>>& BVHs, const AABB& parentAABB)
    {
        EK_CORE_PROFILE();

        if (BVHs.empty())
            return nullptr;

        if (BVHs.size() == 1)
        {
            return BVHs[0]->Build(&m_indexCounter);
        }
        else
        {
            Ref<BVH::Node> node = CreateRef<BVH::Node>();
            node->min = parentAABB.GetMin();
            node->max = parentAABB.GetMax();
            node->meshIndex = -1;
            node->isLeaf = 0;
            node->index = m_indexCounter++;

            BVH::SplitResponse response = FindSplit(BVHs);

            Vec<Ref<BVH>> leftBVH, rightBVH;
            AABB leftAABB, rightAABB;
            for (const auto& BVH : BVHs)
            {
                if (BVH->GetAABB().Center()[response.axis] < response.splitPos)
                {
                    leftBVH.push_back(BVH);
                    leftAABB.Expand(BVH->GetAABB());
                }
                else
                {
                    rightBVH.push_back(BVH);
                    rightAABB.Expand(BVH->GetAABB());
                }
            }

            node->left = BuildRecursive(leftBVH, leftAABB);
            node->right = BuildRecursive(rightBVH, rightAABB);
            return node;
        }
    }

    BVH::SplitResponse SceneBVH::FindSplit(const Vec<Ref<BVH>>& BVHs)
    {
        EK_CORE_PROFILE();

        Vec<AABB> boxes;
        for (const auto& BVH : BVHs)
        {
            boxes.push_back(BVH->GetAABB());
        }
        return BVH::FindSplit(boxes);
    }
}
