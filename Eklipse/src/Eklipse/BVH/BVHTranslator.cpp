/**********************************************************************
Copyright (c) 2016 Advanced Micro Devices, Inc. All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
********************************************************************/

// Modified code from AMD RadeonRays to fit the Eklipse engine
// Original code can be found at: https://github.com/knightcrawler25/GLSL-PathTracer

#include "precompiled.h"
#include "BVHTranslator.h"

#include <Eklipse/Renderer/Mesh.h>

namespace Eklipse
{
    BVHTranslator::BVHTranslator(Scene* scene) : m_scene(scene), m_topLevelBvh(nullptr) {}

    void BVHTranslator::Process()
    {
        EK_CORE_PROFILE();
        EK_CORE_TRACE("Processing BVH");

        m_meshes.clear();
        m_meshInstances.clear();

        m_topLevelBvh = CreateUnique<BVH>(10.0f, 64, false);

        auto meshInstances = m_scene->GetRegistry().view<RayTracingMeshComponent>();
        meshInstances.each([&](auto entity, RayTracingMeshComponent& mesh)
        {
            Entity e = Entity(entity, m_scene);
            auto meshPtr = e.GetComponent<MeshComponent>().mesh;

            bool found = false;
            for (auto& m : m_meshes)
            {
                if (m == meshPtr)
                {
                    found = true;
                    break;
                }
            }
            if (!found)
                m_meshes.push_back(meshPtr);

            m_meshInstances.push_back(mesh.index);
        });

        Vec<BoundingBox> meshBounds;
        for (auto mesh : m_meshes)
        {
            mesh->BuildBVH();
            meshBounds.push_back(mesh->GetBVH()->GetBounds());
        }
        m_topLevelBvh->Build(meshBounds.data(), meshBounds.size());

        ProcessBLAS();
        ProcessTLAS();

        EK_CORE_DBG("BVH Processed");
    }
    Vec<BVHTranslator::Node>& BVHTranslator::GetNodes()
    {
        return m_nodes;
    }
    int BVHTranslator::GetTopLevelIndex() const
    {
        return m_topLevelIndex;
    }

    void BVHTranslator::ProcessBLAS()
    {
        EK_CORE_PROFILE();

        int nodeCnt = 0;
        for (auto mesh : m_meshes)
        {
            nodeCnt += mesh->GetBVH()->m_nodecnt;
        }
        m_topLevelIndex = nodeCnt;

        nodeCnt += 2 * m_meshInstances.size();
        m_nodes.resize(nodeCnt);

        int bvhRootIndex = 0;
        m_curTriIndex = 0;

        for (auto mesh : m_meshes)
        {
            m_curNode = bvhRootIndex;

            m_bvhRootStartIndices.push_back(bvhRootIndex);
            bvhRootIndex += mesh->GetBVH()->m_nodecnt;

            ProcessBLASNodes(mesh->GetBVH()->m_root);
            m_curTriIndex += mesh->GetBVH()->GetNumIndices();
        }
    }
    void BVHTranslator::ProcessTLAS()
    {
        EK_CORE_PROFILE();

        m_curNode = m_topLevelIndex;
        ProcessTLASNodes(m_topLevelBvh->m_root);
    }
    int BVHTranslator::ProcessBLASNodes(const BVH::Node* node)
    {
        EK_CORE_PROFILE();

        BoundingBox box = node->bounds;

        m_nodes[m_curNode].boundingBoxMin = box.m_pMin;
        m_nodes[m_curNode].boundingBoxMax = box.m_pMax;
        m_nodes[m_curNode].LRLeaf.z = 0;

        int index = m_curNode;

        if (node->type == BVH::NodeType::Leaf)
        {
            m_nodes[m_curNode].LRLeaf.x = node->startidx + m_curTriIndex;
            m_nodes[m_curNode].LRLeaf.y = node->numprims;
            m_nodes[m_curNode].LRLeaf.z = 1;
        }
        else
        {
            m_curNode++;
            m_nodes[index].LRLeaf.x = ProcessBLASNodes(node->lc);
            m_curNode++;
            m_nodes[index].LRLeaf.y = ProcessBLASNodes(node->rc);
        }
        return index;
    }
    int BVHTranslator::ProcessTLASNodes(const BVH::Node* node)
    {
        EK_CORE_PROFILE();

        BoundingBox box = node->bounds;

        m_nodes[m_curNode].boundingBoxMin = box.m_pMin;
        m_nodes[m_curNode].boundingBoxMax = box.m_pMax;
        m_nodes[m_curNode].LRLeaf.z = 0;

        int index = m_curNode;

        if (node->type == BVH::NodeType::Leaf)
        {
            int instanceIndex = m_topLevelBvh->m_packed_indices[node->startidx];
            int meshIndex = m_meshInstances[instanceIndex];

            m_nodes[m_curNode].LRLeaf.x = m_bvhRootStartIndices[meshIndex];
            m_nodes[m_curNode].LRLeaf.y = 0;
            m_nodes[m_curNode].LRLeaf.z = -instanceIndex - 1;
        }
        else
        {
            m_curNode++;
            m_nodes[index].LRLeaf.x = ProcessTLASNodes(node->lc);
            m_curNode++;
            m_nodes[index].LRLeaf.y = ProcessTLASNodes(node->rc);
        }
        return index;
    }
}