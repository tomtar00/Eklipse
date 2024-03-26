#pragma once
#include "BVH.h"

namespace Eklipse
{
    class SplitBVH : public BVH
    {
    protected:
        struct PrimRef
        {
            BoundingBox bounds;
            glm::vec3 center;
            int idx;
        };
        using PrimRefArray = std::vector<PrimRef>;

        enum class SplitType
        {
            Object,
            Spatial
        };

    public:
        SplitBVH(float traversal_cost, int num_bins, int max_split_depth,
            float min_overlap,float extra_refs_budget) : BVH(traversal_cost, num_bins, true),
            m_max_split_depth(max_split_depth), m_min_overlap(min_overlap), 
            m_extra_refs_budget(extra_refs_budget), m_num_nodes_required(0), 
            m_num_nodes_for_regular(0), m_num_nodes_archived(0) {}

        SplitBVH(SplitBVH const&) = delete;
        SplitBVH& operator = (SplitBVH const&) = delete;

    protected:
        void BuildNode(SplitRequest& req, PrimRefArray& primrefs);

        SahSplit FindObjectSahSplit(SplitRequest const& req, PrimRefArray const& refs) const;
        SahSplit FindSpatialSahSplit(SplitRequest const& req, PrimRefArray const& refs) const;

        void SplitPrimRefs(SahSplit const& split, SplitRequest const& req, PrimRefArray& refs, int& extra_refs);
        bool SplitPrimRef(PrimRef const& ref, int axis, float split, PrimRef& leftref, PrimRef& rightref) const;

    protected:
        void BuildImpl(BoundingBox const* bounds, int numbounds) override;
        Node* AllocateNode() override;
        void  InitNodeAllocator(size_t maxnum) override;

    private:
        int m_max_split_depth;
        float m_min_overlap;
        float m_extra_refs_budget;
        int m_num_nodes_required;
        int m_num_nodes_for_regular;
        int m_num_nodes_archived;

        std::list<std::vector<Node>> m_node_archive;
    };
}