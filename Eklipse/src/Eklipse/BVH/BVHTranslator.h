#pragma once

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

#include "BVH.h"

#include <Eklipse/Scene/Scene.h>
#include <Eklipse/Scene/Components.h>

namespace Eklipse
{
    class Mesh;

    class BVHTranslator
    {
    public:
        struct Node
        {
            glm::vec3 boundingBoxMin;   float __pad0;
            glm::vec3 boundingBoxMax;   float __pad1;
            glm::vec3 LRLeaf;           float __pad2;
        };

    public:
        BVHTranslator(Scene* scene);

        void Process();
        Vec<Node>& GetNodes();
        int GetTopLevelIndex() const;
        
    private:
        void ProcessBLAS();
        void ProcessTLAS();
        int ProcessBLASNodes(const BVH::Node* node);
        int ProcessTLASNodes(const BVH::Node* node);

    private:
        int m_topLevelIndex = 0;
        int m_curNode = 0;
        int m_curTriIndex = 0;

        Unique<BVH> m_topLevelBvh;
        Vec<Node> m_nodes;
        Vec<int> m_bvhRootStartIndices;

        Vec<Mesh*> m_meshes;
        Vec<uint32_t> m_meshInstances;
        
        Scene* m_scene;
    };
}
