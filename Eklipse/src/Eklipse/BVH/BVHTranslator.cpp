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

namespace Eklipse
{
    BVHTranslator::BVHTranslator(Scene* scene) : m_scene(scene) {}

    void BVHTranslator::ProcessBLAS()
    {
        int nodeCnt = 0;

        Vec<Mesh*> meshes;
        auto meshInstances = m_scene->GetRegistry().view<RayTracingMeshComponent>();
        meshInstances.each([&](auto entity, RayTracingMeshComponent& mesh)
        {
            Entity e = Entity(entity, m_scene);
            auto meshPtr = e.GetComponent<MeshComponent>().mesh;
            nodeCnt += meshPtr->GetBVH()->m_nodecnt;

            bool found = false;
            for (auto& m : meshes)
            {
                if (m == meshPtr)
                {
                    found = true;
                    break;
                }
            }
            if (!found)
                meshes.push_back(meshPtr);

            m_meshInstances[m_meshInstances.size()] = &mesh;
        });

        topLevelIndex = nodeCnt;

        nodeCnt += 2 * meshInstances.size();
        nodes.resize(nodeCnt);

        int bvhRootIndex = 0;
        curTriIndex = 0;

        for (int i = 0; i < meshes.size(); i++)
        {
            Mesh* mesh = meshes[i];
            curNode = bvhRootIndex;

            bvhRootStartIndices.push_back(bvhRootIndex);
            bvhRootIndex += mesh->GetBVH()->m_nodecnt;

            ProcessBLASNodes(mesh->GetBVH()->m_root);
            curTriIndex += mesh->GetBVH()->GetNumIndices();
        }
    }
    void BVHTranslator::ProcessTLAS()
    {
        curNode = topLevelIndex;
        ProcessTLASNodes(topLevelBvh->m_root);
    }
    void BVHTranslator::UpdateTLAS()
    {
        topLevelBvh = topLevelBvh;
        curNode = topLevelIndex;
        ProcessTLASNodes(topLevelBvh->m_root);
    }
    void BVHTranslator::Process()
    {
        topLevelBvh = topLevelBvh;
        ProcessBLAS();
        ProcessTLAS();
    }

    int BVHTranslator::ProcessBLASNodes(const BVH::Node* node)
    {
        BoundingBox box = node->bounds;

        nodes[curNode].boundingBoxMin = box.m_pMin;
        nodes[curNode].boundingBoxMax = box.m_pMax;
        nodes[curNode].LRLeaf.z = 0;

        int index = curNode;

        if (node->type == BVH::NodeType::Leaf)
        {
            nodes[curNode].LRLeaf.x = curTriIndex + node->startidx;
            nodes[curNode].LRLeaf.y = node->numprims;
            nodes[curNode].LRLeaf.z = 1;
        }
        else
        {
            curNode++;
            nodes[index].LRLeaf.x = ProcessBLASNodes(node->lc);
            curNode++;
            nodes[index].LRLeaf.y = ProcessBLASNodes(node->rc);
        }
        return index;
    }
    int BVHTranslator::ProcessTLASNodes(const BVH::Node* node)
    {
        BoundingBox box = node->bounds;

        nodes[curNode].boundingBoxMin = box.m_pMin;
        nodes[curNode].boundingBoxMax = box.m_pMax;
        nodes[curNode].LRLeaf.z = 0;

        int index = curNode;

        if (node->type == BVH::NodeType::Leaf)
        {
            int instanceIndex = topLevelBvh->m_packed_indices[node->startidx];
            int meshIndex = m_meshInstances[instanceIndex]->index;

            nodes[curNode].LRLeaf.x = bvhRootStartIndices[meshIndex];
            nodes[curNode].LRLeaf.y = 0;
            nodes[curNode].LRLeaf.z = -instanceIndex - 1;
        }
        else
        {
            curNode++;
            nodes[index].LRLeaf.x = ProcessTLASNodes(node->lc);
            curNode++;
            nodes[index].LRLeaf.y = ProcessTLASNodes(node->rc);
        }
        return index;
    }
}