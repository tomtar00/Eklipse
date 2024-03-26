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
#include "BVH.h"

#define MAX_PRIMITIVES_PER_LEAF 1

static bool is_nan(float v)
{
    return v != v;
}

namespace Eklipse
{
    void BVH::Build(const BoundingBox* bounds, int numBounds)
    {
        for (int i = 0; i < numBounds; ++i)
        {
            m_bounds.Grow(bounds[i]);
        }

        BuildImpl(bounds, numBounds);
    }
    const BoundingBox& BVH::GetBounds() const
    {
        return m_bounds;
    }
    int BVH::GetHeight() const
    {
        return 0;
    }
    void BVH::InitNodeAllocator(size_t maxnum)
    {
        m_nodecnt = 0;
        m_nodes.resize(maxnum);
    }
    BVH::Node* BVH::AllocateNode()
    {
        return &m_nodes[m_nodecnt++];
    }
    void BVH::BuildNode(const SplitRequest & req, const BoundingBox* bounds, const glm::vec3* centroids, int* primindices)
    {
        m_height = std::max(m_height, req.level);

        Node* node = AllocateNode();
        node->bounds = req.bounds;
        node->index = req.index;

        if (req.numprims < 2)
        {
#ifdef USE_TBB
            primitive_mutex_.lock();
#endif
            node->type = Leaf;
            node->startidx = static_cast<int>(m_packed_indices.size());
            node->numprims = req.numprims;

            for (auto i = 0; i < req.numprims; ++i)
            {
                m_packed_indices.push_back(primindices[req.startidx + i]);
            }
#ifdef USE_TBB
            primitive_mutex_.unlock();
#endif
        }
        else
        {
            int axis = req.centroid_bounds.MaxDim();
            float border = req.centroid_bounds.Center()[axis];

            if (m_usesah)
            {
                SahSplit ss = FindSahSplit(req, bounds, centroids, primindices);

                if (!is_nan(ss.split))
                {
                    axis = ss.dim;
                    border = ss.split;

                    if (req.numprims < ss.sah && req.numprims < MAX_PRIMITIVES_PER_LEAF)
                    {
                        node->type = Leaf;
                        node->startidx = static_cast<int>(m_packed_indices.size());
                        node->numprims = req.numprims;

                        for (auto i = 0; i < req.numprims; ++i)
                        {
                            m_packed_indices.push_back(primindices[req.startidx + i]);
                        }

                        if (req.ptr) *req.ptr = node;
                        return;
                    }
                }
            }

            node->type = Internal;

            BoundingBox leftbounds, rightbounds, leftcentroid_bounds, rightcentroid_bounds;
            int splitidx = req.startidx;

            bool near2far = (req.numprims + req.startidx) & 0x1;

            if (req.centroid_bounds.Extents()[axis] > 0.f)
            {
                auto first = req.startidx;
                auto last = req.startidx + req.numprims;

                if (near2far)
                {
                    while (true)
                    {
                        while ((first != last) &&
                            centroids[primindices[first]][axis] < border)
                        {
                            leftbounds.Grow(bounds[primindices[first]]);
                            leftcentroid_bounds.Grow(centroids[primindices[first]]);
                            ++first;
                        }

                        if (first == last--) break;

                        rightbounds.Grow(bounds[primindices[first]]);
                        rightcentroid_bounds.Grow(centroids[primindices[first]]);

                        while ((first != last) &&
                            centroids[primindices[last]][axis] >= border)
                        {
                            rightbounds.Grow(bounds[primindices[last]]);
                            rightcentroid_bounds.Grow(centroids[primindices[last]]);
                            --last;
                        }

                        if (first == last) break;

                        leftbounds.Grow(bounds[primindices[last]]);
                        leftcentroid_bounds.Grow(centroids[primindices[last]]);

                        std::swap(primindices[first++], primindices[last]);
                    }
                }
                else
                {
                    while (true)
                    {
                        while ((first != last) &&
                            centroids[primindices[first]][axis] >= border)
                        {
                            leftbounds.Grow(bounds[primindices[first]]);
                            leftcentroid_bounds.Grow(centroids[primindices[first]]);
                            ++first;
                        }

                        if (first == last--) break;

                        rightbounds.Grow(bounds[primindices[first]]);
                        rightcentroid_bounds.Grow(centroids[primindices[first]]);

                        while ((first != last) &&
                            centroids[primindices[last]][axis] < border)
                        {
                            rightbounds.Grow(bounds[primindices[last]]);
                            rightcentroid_bounds.Grow(centroids[primindices[last]]);
                            --last;
                        }

                        if (first == last) break;

                        leftbounds.Grow(bounds[primindices[last]]);
                        leftcentroid_bounds.Grow(centroids[primindices[last]]);

                        std::swap(primindices[first++], primindices[last]);
                    }
                }

                splitidx = first;
            }

            if (splitidx == req.startidx || splitidx == req.startidx + req.numprims)
            {
                splitidx = req.startidx + (req.numprims >> 1);

                for (int i = req.startidx; i < splitidx; ++i)
                {
                    leftbounds.Grow(bounds[primindices[i]]);
                    leftcentroid_bounds.Grow(centroids[primindices[i]]);
                }

                for (int i = splitidx; i < req.startidx + req.numprims; ++i)
                {
                    rightbounds.Grow(bounds[primindices[i]]);
                    rightcentroid_bounds.Grow(centroids[primindices[i]]);
                }
            }

            SplitRequest leftrequest = { req.startidx, splitidx - req.startidx, &node->lc, leftbounds, leftcentroid_bounds, req.level + 1, (req.index << 1) };
            SplitRequest rightrequest = { splitidx, req.numprims - (splitidx - req.startidx), &node->rc, rightbounds, rightcentroid_bounds, req.level + 1, (req.index << 1) + 1 };

            {
                BuildNode(leftrequest, bounds, centroids, primindices);
            }

            {
                BuildNode(rightrequest, bounds, centroids, primindices);
            }
        }

        if (req.ptr) *req.ptr = node;
    }
    BVH::SahSplit BVH::FindSahSplit(const SplitRequest& req, const BoundingBox* bounds, const glm::vec3* centroids, int* primindices) const
    {
        int splitidx = -1;
        float sah = std::numeric_limits<float>::max();
        SahSplit split;
        split.dim = 0;
        split.split = std::numeric_limits<float>::quiet_NaN();

        glm::vec3 centroid_extents = req.centroid_bounds.Extents();
        if (glm::dot(centroid_extents, centroid_extents) == 0.f)
        {
            return split;
        }

        struct Bin
        {
            BoundingBox bounds;
            int count;
        };

        std::vector<Bin> bins[3];
        bins[0].resize(m_num_bins);
        bins[1].resize(m_num_bins);
        bins[2].resize(m_num_bins);

        float invarea = 1.f / req.bounds.SurfaceArea();
        glm::vec3 rootmin = req.centroid_bounds.m_pMin;

        for (int axis = 0; axis < 3; ++axis)
        {
            float rootminc = rootmin[axis];
            float centroid_rng = centroid_extents[axis];
            float invcentroid_rng = 1.f / centroid_rng;

            if (centroid_rng == 0.f) continue;

            for (int i = 0; i < m_num_bins; ++i)
            {
                bins[axis][i].count = 0;
                bins[axis][i].bounds = BoundingBox();
            }

            for (int i = req.startidx; i < req.startidx + req.numprims; ++i)
            {
                int idx = primindices[i];
                int binidx = (int)std::min<float>(static_cast<float>(m_num_bins) * ((centroids[idx][axis] - rootminc) * invcentroid_rng), static_cast<float>(m_num_bins - 1));

                ++bins[axis][binidx].count;
                bins[axis][binidx].bounds.Grow(bounds[idx]);
            }

            std::vector<BoundingBox> rightbounds(m_num_bins - 1);

            BoundingBox rightbox = BoundingBox();
            for (int i = m_num_bins - 1; i > 0; --i)
            {
                rightbox.Grow(bins[axis][i].bounds);
                rightbounds[i - 1] = rightbox;
            }

            BoundingBox leftbox = BoundingBox();
            int  leftcount = 0;
            int  rightcount = req.numprims;

            float sahtmp = 0.f;
            for (int i = 0; i < m_num_bins - 1; ++i)
            {
                leftbox.Grow(bins[axis][i].bounds);
                leftcount += bins[axis][i].count;
                rightcount -= bins[axis][i].count;

                sahtmp = m_traversal_cost + (leftcount * leftbox.SurfaceArea() + rightcount * rightbounds[i].SurfaceArea()) * invarea;

                if (sahtmp < sah)
                {
                    split.dim = axis;
                    splitidx = i;
                    split.sah = sah = sahtmp;
                }
            }
        }

        if (splitidx != -1)
        {
            split.split = rootmin[split.dim] + (splitidx + 1) * (centroid_extents[split.dim] / m_num_bins);
        }

        return split;
    }
    void BVH::BuildImpl(const BoundingBox* bounds, int numbounds)
    {
        InitNodeAllocator(2 * numbounds - 1);

        std::vector<glm::vec3> centroids(numbounds);
        m_indices.resize(numbounds);
        std::iota(m_indices.begin(), m_indices.end(), 0);

        BoundingBox centroid_bounds;
        for (size_t i = 0; i < static_cast<size_t>(numbounds); ++i)
        {
            glm::vec3 c = bounds[i].Center();
            centroid_bounds.Grow(c);
            centroids[i] = c;
        }

        SplitRequest init = { 0, numbounds, nullptr, m_bounds, centroid_bounds, 0, 1 };

#ifdef USE_BUILD_STACK
        std::stack<SplitRequest> stack;
        stack.push(init);

        while (!stack.empty())
        {
            SplitRequest req = stack.top();
            stack.pop();

            Node* node = AllocateNode();
            node->bounds = req.bounds;

            if (req.numprims < 2)
            {
                node->type = Leaf;
                node->startidx = (int)primitives_.size();
                node->numprims = req.numprims;
                for (int i = req.startidx; i < req.startidx + req.numprims; ++i)
                {
                    primitives_.push_back(prims[primindices[i]]);
                }
            }
            else
            {
                node->type = Internal;

                int axis = req.centroid_bounds.MaxDim();
                float border = req.centroid_bounds.Center()[axis];

                BoundingBox leftbounds, rightbounds, leftcentroid_bounds, rightcentroid_bounds;
                int splitidx = 0;

                if (req.centroid_bounds.extents()[axis] > 0.f)
                {

                    auto first = req.startidx;
                    auto last = req.startidx + req.numprims;

                    while (1)
                    {
                        while ((first != last) &&
                            centroids[primindices[first]][axis] < border)
                        {
                            leftbounds.Grow(bounds[primindices[first]]);
                            leftcentroid_bounds.Grow(centroids[primindices[first]]);
                            ++first;
                        }

                        if (first == last--) break;

                        rightbounds.Grow(bounds[primindices[first]]);
                        rightcentroid_bounds.Grow(centroids[primindices[first]]);

                        while ((first != last) &&
                            centroids[primindices[last]][axis] >= border)
                        {
                            rightbounds.Grow(bounds[primindices[last]]);
                            rightcentroid_bounds.Grow(centroids[primindices[last]]);
                            --last;
                        }

                        if (first == last) break;

                        leftbounds.Grow(bounds[primindices[last]]);
                        leftcentroid_bounds.Grow(centroids[primindices[last]]);

                        std::swap(primindices[first++], primindices[last]);
                    }

                    splitidx = first;
                }
                else
                {
                    splitidx = req.startidx + (req.numprims >> 1);

                    for (int i = req.startidx; i < splitidx; ++i)
                    {
                        leftbounds.Grow(bounds[primindices[i]]);
                        leftcentroid_bounds.Grow(centroids[primindices[i]]);
                    }

                    for (int i = splitidx; i < req.startidx + req.numprims; ++i)
                    {
                        rightbounds.Grow(bounds[primindices[i]]);
                        rightcentroid_bounds.Grow(centroids[primindices[i]]);
                    }
                }

                SplitRequest leftrequest = { req.startidx, splitidx - req.startidx, &node->lc, leftbounds, leftcentroid_bounds };
                SplitRequest rightrequest = { splitidx, req.numprims - (splitidx - req.startidx), &node->rc, rightbounds, rightcentroid_bounds };

                stack.push(leftrequest);
                stack.push(rightrequest);
            }

            if (req.ptr) *req.ptr = node;
        }
#else
        BuildNode(init, bounds, &centroids[0], &m_indices[0]);
#endif

        m_root = &m_nodes[0];
    }
}
