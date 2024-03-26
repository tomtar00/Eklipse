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

#include <iostream>
#include "BoundingBox.h"

namespace Eklipse
{
    class BVH
    {
    public:
        enum NodeType
        {
            Internal,
            Leaf
        };
        struct Node
        {
            BoundingBox bounds;
            NodeType type;
            int index;

            union
            {
                struct
                {
                    Node* lc;
                    Node* rc;
                };

                struct
                {
                    int startidx;
                    int numprims;
                };
            };
        };

        BVH(float traversal_cost, int num_bins = 64, bool usesah = false)
            : m_root(nullptr), m_num_bins(num_bins), m_usesah(usesah), 
            m_height(0), m_traversal_cost(traversal_cost) {}

        void Build(const BoundingBox* bounds, int numBounds);

        const BoundingBox& GetBounds() const;
        int GetHeight() const;

        virtual int const* GetIndices() const;
        virtual size_t GetNumIndices() const;

    protected:
        virtual void BuildImpl(const BoundingBox* bounds, int numBounds);
        virtual Node* AllocateNode();
        virtual void  InitNodeAllocator(size_t maxNum);

        struct SplitRequest
        {
            int startidx;
            int numprims;
            Node** ptr;
            BoundingBox bounds;
            BoundingBox centroid_bounds;
            int level;
            int index;
        };

        struct SahSplit
        {
            int dim;
            float split;
            float sah;
            float overlap;
        };

        void BuildNode(const SplitRequest& req, const BoundingBox* bounds, const glm::vec3* centroids, int* primindices);
        SahSplit FindSahSplit(const SplitRequest& req, const BoundingBox* bounds, const glm::vec3* centroids, int* primindices) const;

        std::vector<Node> m_nodes;
        std::vector<int> m_indices;
        std::atomic<int> m_nodecnt;

        std::vector<int> m_packed_indices;

        BoundingBox m_bounds;
        Node* m_root;
        bool m_usesah;
        int m_height;
        float m_traversal_cost;
        int m_num_bins;

    private:
        BVH(const BVH&) = delete;
        BVH& operator = (const BVH &) = delete;

        friend class BVHTranslator;
    };
}