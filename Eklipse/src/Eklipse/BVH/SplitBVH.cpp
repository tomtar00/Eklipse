#include "precompiled.h"
#include "SplitBVH.h"

namespace Eklipse
{
    void SplitBVH::BuildImpl(BoundingBox const* bounds, int numbounds)
    {
        EK_CORE_PROFILE();

        PrimRefArray primrefs(numbounds);

        std::vector<glm::vec3> centroids(numbounds);
        BoundingBox centroid_bounds;

        for (auto i = 0; i < numbounds; ++i)
        {
            primrefs[i] = PrimRef{ bounds[i], bounds[i].Center(), i };

            auto c = bounds[i].Center();
            centroid_bounds.Grow(c);
        }

        m_num_nodes_for_regular = (2 * numbounds - 1);
        m_num_nodes_required = (int)(m_num_nodes_for_regular * (1.f + m_extra_refs_budget));

        InitNodeAllocator(m_num_nodes_required);

        SplitRequest init = { 0, numbounds, nullptr, m_bounds, centroid_bounds, 0 };

        BuildNode(init, primrefs);
    }
    void SplitBVH::BuildNode(SplitRequest& req, PrimRefArray& primrefs)
    {
        EK_CORE_PROFILE();

        m_height = std::max(m_height, req.level);

        Node* node = AllocateNode();
        node->bounds = req.bounds;

        if (req.numprims < 4)
        {
            node->type = Leaf;
            node->startidx = (int)m_packed_indices.size();
            node->numprims = req.numprims;

            for (int i = req.startidx; i < req.startidx + req.numprims; ++i)
            {
                m_packed_indices.push_back(primrefs[i].idx);
            }
        }
        else
        {
            node->type = Internal;

            int axis = req.centroid_bounds.MaxDim();
            float border = req.centroid_bounds.Center()[axis];

            SahSplit os = FindObjectSahSplit(req, primrefs);
            SahSplit ss;
            auto split_type = SplitType::Object;

            if (req.level < m_max_split_depth && m_nodecnt < m_num_nodes_required && os.overlap > m_min_overlap)
            {
                ss = FindSpatialSahSplit(req, primrefs);

                if (!isnan(ss.split) &&
                    ss.sah < os.sah)
                {
                    split_type = SplitType::Spatial;
                }
            }

            if (split_type == SplitType::Spatial)
            {
                size_t elems = req.startidx + req.numprims * 2;
                if (primrefs.size() < elems)
                {
                    primrefs.resize(elems);
                }

                int extra_refs = 0;
                SplitPrimRefs(ss, req, primrefs, extra_refs);
                req.numprims += extra_refs;
                border = ss.split;
                axis = ss.dim;
            }
            else
            {
                border = !isnan(os.split) ? os.split : border;
                axis = !isnan(os.split) ? os.dim : axis;
            }

            BoundingBox leftbounds, rightbounds, leftcentroid_bounds, rightcentroid_bounds;
            int splitidx = req.startidx;

            bool near2far = (req.numprims + req.startidx) & 0x1;

            bool(*cmpl)(float, float) = [](float a, float b) -> bool { return a < b; };
            bool(*cmpge)(float, float) = [](float a, float b) -> bool { return a >= b; };
            auto cmp1 = near2far ? cmpl : cmpge;
            auto cmp2 = near2far ? cmpge : cmpl;

            if (req.centroid_bounds.Extents()[axis] > 0.f)
            {
                auto first = req.startidx;
                auto last = req.startidx + req.numprims;

                while (true)
                {
                    while ((first != last) && cmp1(primrefs[first].center[axis], border))
                    {
                        leftbounds.Grow(primrefs[first].bounds);
                        leftcentroid_bounds.Grow(primrefs[first].center);
                        ++first;
                    }

                    if (first == last--) break;

                    rightbounds.Grow(primrefs[first].bounds);
                    rightcentroid_bounds.Grow(primrefs[first].center);

                    while ((first != last) && cmp2(primrefs[last].center[axis], border))
                    {
                        rightbounds.Grow(primrefs[last].bounds);
                        rightcentroid_bounds.Grow(primrefs[last].center);
                        --last;
                    }

                    if (first == last) break;

                    leftbounds.Grow(primrefs[last].bounds);
                    leftcentroid_bounds.Grow(primrefs[last].center);

                    std::swap(primrefs[first++], primrefs[last]);
                }


                splitidx = first;
            }


            if (splitidx == req.startidx || splitidx == req.startidx + req.numprims)
            {
                splitidx = req.startidx + (req.numprims >> 1);

                for (int i = req.startidx; i < splitidx; ++i)
                {
                    leftbounds.Grow(primrefs[i].bounds);
                    leftcentroid_bounds.Grow(primrefs[i].center);
                }

                for (int i = splitidx; i < req.startidx + req.numprims; ++i)
                {
                    rightbounds.Grow(primrefs[i].bounds);
                    rightcentroid_bounds.Grow(primrefs[i].center);
                }
            }

            SplitRequest leftrequest = { req.startidx, splitidx - req.startidx, &node->lc, leftbounds, leftcentroid_bounds, req.level + 1 };
            SplitRequest rightrequest = { splitidx, req.numprims - (splitidx - req.startidx), &node->rc, rightbounds, rightcentroid_bounds, req.level + 1 };


            {
                BuildNode(rightrequest, primrefs);
            }

            {
                BuildNode(leftrequest, primrefs);
            }
        }

        if (req.ptr) *req.ptr = node;
    }
    BVH::SahSplit SplitBVH::FindObjectSahSplit(SplitRequest const& req, PrimRefArray const& refs) const
    {
        EK_CORE_PROFILE();

        int splitidx = -1;
        auto sah = std::numeric_limits<float>::max();
        SahSplit split;
        split.dim = 0;
        split.split = std::numeric_limits<float>::quiet_NaN();
        split.sah = sah;

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

        auto invarea = 1.f / req.bounds.SurfaceArea();
        auto rootmin = req.centroid_bounds.m_pMin;

        for (int axis = 0; axis < 3; ++axis)
        {
            float rootminc = rootmin[axis];
            auto centroid_rng = centroid_extents[axis];
            auto invcentroid_rng = 1.f / centroid_rng;

            if (centroid_rng == 0.f) continue;

            for (int i = 0; i < m_num_bins; ++i)
            {
                bins[axis][i].count = 0;
                bins[axis][i].bounds = BoundingBox();
            }

            for (int i = req.startidx; i < req.startidx + req.numprims; ++i)
            {
                auto idx = i;
                auto binidx = (int)std::min<float>(static_cast<float>(m_num_bins) * ((refs[idx].center[axis] - rootminc) * invcentroid_rng), static_cast<float>(m_num_bins - 1));

                ++bins[axis][binidx].count;
                bins[axis][binidx].bounds.Grow(refs[idx].bounds);
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
                    sah = sahtmp;

                    split.overlap = BoundingBox::Intersection(leftbox, rightbounds[i]).SurfaceArea() * invarea;
                }
            }
        }

        if (splitidx != -1)
        {
            split.split = rootmin[split.dim] + (splitidx + 1) * (centroid_extents[split.dim] / m_num_bins);
            split.sah = sah;

        }

        return split;
    }
    BVH::SahSplit SplitBVH::FindSpatialSahSplit(SplitRequest const& req, PrimRefArray const& refs) const
    {
        EK_CORE_PROFILE();

        int const kNumBins = 128;
        auto sah = std::numeric_limits<float>::max();
        SahSplit split;
        split.dim = 0;
        split.split = std::numeric_limits<float>::quiet_NaN();
        split.sah = sah;

        glm::vec3 extents = req.bounds.Extents();
        auto invarea = 1.f / req.bounds.SurfaceArea();

        if (glm::dot(extents, extents) == 0.f)
        {
            return split;
        }

        struct Bin
        {
            BoundingBox bounds;
            int enter;
            int exit;
        };

        Bin bins[3][kNumBins];

        glm::vec3 origin = req.bounds.m_pMin;
        glm::vec3 binsize = req.bounds.Extents() * (1.f / kNumBins);
        glm::vec3 invbinsize = glm::vec3(1.f / binsize.x, 1.f / binsize.y, 1.f / binsize.z);

        for (int axis = 0; axis < 3; ++axis)
        {
            for (int i = 0; i < kNumBins; ++i)
            {
                bins[axis][i].bounds = BoundingBox();
                bins[axis][i].enter = 0;
                bins[axis][i].exit = 0;
            }
        }

        for (int i = req.startidx; i < req.startidx + req.numprims; ++i)
        {
            PrimRef const& primref(refs[i]);
            glm::vec3 firstbin = glm::clamp((primref.bounds.m_pMin - origin) * invbinsize, glm::vec3(0, 0, 0), glm::vec3(kNumBins - 1, kNumBins - 1, kNumBins - 1));
            glm::vec3 lastbin = glm::clamp((primref.bounds.m_pMax - origin) * invbinsize, firstbin, glm::vec3(kNumBins - 1, kNumBins - 1, kNumBins - 1));
            for (int axis = 0; axis < 3; ++axis)
            {
                if (extents[axis] == 0.f) continue;
                auto tempref = primref;

                for (int j = (int)firstbin[axis]; j < (int)lastbin[axis]; ++j)
                {
                    PrimRef leftref, rightref;
                    float splitval = origin[axis] + binsize[axis] * (j + 1);
                    if (SplitPrimRef(tempref, axis, splitval, leftref, rightref))
                    {
                        bins[axis][j].bounds.Grow(leftref.bounds);
                        tempref = rightref;
                    }
                }
                bins[axis][(int)lastbin[axis]].bounds.Grow(tempref.bounds);
                bins[axis][(int)firstbin[axis]].enter++;
                bins[axis][(int)lastbin[axis]].exit++;
            }
        }

        BoundingBox rightbounds[kNumBins - 1];
        split.sah = std::numeric_limits<float>::max();

        for (int axis = 0; axis < 3; ++axis)
        {
            if (extents[axis] == 0.f)
                continue;

            BoundingBox rightbox = BoundingBox();
            for (int i = kNumBins - 1; i > 0; --i)
            {
                rightbox = BoundingBox::Union(rightbox, bins[axis][i].bounds);
                rightbounds[i - 1] = rightbox;
            }

            BoundingBox leftbox = BoundingBox();
            int  leftcount = 0;
            int  rightcount = req.numprims;

            for (int i = 1; i < kNumBins; ++i)
            {
                leftbox.Grow(bins[axis][i - 1].bounds);
                leftcount += bins[axis][i - 1].enter;
                rightcount -= bins[axis][i - 1].exit;
                float sah = m_traversal_cost + (leftbox.SurfaceArea() *
                    +rightbounds[i - 1].SurfaceArea() * rightcount) * invarea;

                if (sah < split.sah)
                {
                    split.sah = sah;
                    split.dim = axis;
                    split.split = origin[axis] + binsize[axis] * (float)i;

                    split.overlap = 0.f;
                }
            }
        }

        return split;
    }
    void SplitBVH::SplitPrimRefs(SahSplit const& split, SplitRequest const& req, PrimRefArray& refs, int& extra_refs)
    {
        EK_CORE_PROFILE();

        int appendprims = req.numprims;

        for (int i = req.startidx; i < req.startidx + req.numprims; ++i)
        {
            assert(static_cast<size_t>(req.startidx + appendprims) < refs.size());

            PrimRef leftref, rightref;
            if (SplitPrimRef(refs[i], split.dim, split.split, leftref, rightref))
            {
                refs[i] = leftref;
                refs[req.startidx + appendprims++] = rightref;
            }
        }

        extra_refs = appendprims - req.numprims;
    }
    bool SplitBVH::SplitPrimRef(PrimRef const& ref, int axis, float split, PrimRef& leftref, PrimRef& rightref) const
    {
        EK_CORE_PROFILE();

        leftref.idx = rightref.idx = ref.idx;
        leftref.bounds = rightref.bounds = ref.bounds;

        if (split > ref.bounds.m_pMin[axis] && split < ref.bounds.m_pMax[axis])
        {
            leftref.bounds.m_pMax[axis] = split;
            rightref.bounds.m_pMin[axis] = split;
            return true;
        }

        return false;
    }
    BVH::Node* SplitBVH::AllocateNode()
    {
        EK_CORE_PROFILE();

        if (m_nodecnt - m_num_nodes_archived >= m_num_nodes_for_regular)
        {
            m_node_archive.push_back(std::move(m_nodes));
            m_num_nodes_archived += m_num_nodes_for_regular;
            m_nodes = std::vector<Node>(m_num_nodes_for_regular);
        }

        return &m_nodes[m_nodecnt++ - m_num_nodes_archived];
    }
    void SplitBVH::InitNodeAllocator(size_t maxnum)
    {
        EK_CORE_PROFILE();

        m_node_archive.clear();
        m_nodecnt = 0;
        m_nodes.resize(maxnum);

        m_root = &m_nodes[0];
    }
}
