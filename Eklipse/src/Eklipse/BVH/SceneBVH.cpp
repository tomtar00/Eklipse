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

        std::queue<Ref<BVH::Node>> q;
        q.push(m_root);

        while (!q.empty()) 
        {
            int levelSize = q.size();

            for (int i = 0; i < levelSize; ++i) 
            {
                Ref<BVH::Node> node = q.front();
                q.pop();

                BVH::FlatNode flatNode{};
                flatNode.min = node->min;
                flatNode.max = node->max;
                flatNode.isLeaf = node->isLeaf;
                flatNode.meshIndex = node->meshIndex;
                flatNode.startTriIndex = m_trianglesCounter;
                flatNode.endTriIndex = m_trianglesCounter + node->triangles.size();
                flatNode.leftChildIndex = 2 * m_flatNodes.size() + 1;
                flatNode.rightChildIndex = 2 * m_flatNodes.size() + 2;
                m_flatNodes.push_back(flatNode);

                if (node->triangles.size() > 0)
                {
                    m_triangles.insert(m_triangles.end(), node->triangles.begin(), node->triangles.end());
                    m_trianglesCounter += node->triangles.size();
                }

                /*EK_CORE_INFO("FlatNode: index: {}, isLeaf: {}, meshIndex: {}, startTriIndex: {}, endTriIndex: {}, leftChildIndex: {}, rightChildIndex: {}",
                    m_flatNodes.size() - 1, flatNode.isLeaf, flatNode.meshIndex, flatNode.startTriIndex, flatNode.endTriIndex, flatNode.leftChildIndex, flatNode.rightChildIndex);*/

                if (node->left)
                    q.push(node->left);
                if (node->right)
                    q.push(node->right);
            }
        }
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

        int axis = FindSplitAxis(BVHs);
        float splitPos = FindSplitPosition(BVHs, axis);

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

    int SceneBVH::FindSplitAxis(const Vec<Ref<BVH>>& BVHs)
    {
        EK_CORE_PROFILE();

        Vec<AABB> boxes;
        for (const auto& BVH : BVHs)
        {
            boxes.push_back(BVH->GetAABB());
        }
        return BVH::FindSplitAxis(boxes);
    }
    float SceneBVH::FindSplitPosition(const Vec<Ref<BVH>>& BVHs, int axis)
    {
        EK_CORE_PROFILE();

        Vec<AABB> boxes;
        for (const auto& BVH : BVHs)
        {
            boxes.push_back(BVH->GetAABB());
        }
        return BVH::FindSplitPosition(boxes, axis);
    }
}
