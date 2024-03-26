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
#include "BoundingBox.h"

namespace Eklipse
{
    glm::vec3 BoundingBox::Center() const
    {
        return (m_pMax + m_pMin) * 0.5f;
    }
    glm::vec3 BoundingBox::Extents() const
    {
        return m_pMax - m_pMin;
    }
    int BoundingBox::MaxDim() const
    {
        glm::vec3 ext = Extents();

        if (ext.x >= ext.y && ext.x >= ext.z)
            return 0;
        if (ext.y >= ext.x && ext.y >= ext.z)
            return 1;
        if (ext.z >= ext.x && ext.z >= ext.y)
            return 2;

        return 0;
    }
    float BoundingBox::SurfaceArea() const
    {
        glm::vec3 ext = Extents();
        return 2.0f * (ext.x * ext.y + ext.x * ext.z + ext.y * ext.z);
    }
    bool BoundingBox::Contains(const glm::vec3& p) const
    {
        glm::vec3 center = Center();
        glm::vec3 radius = Extents() * 0.5f;
        return std::abs(center.x - p.x) <= radius.x &&
            fabs(center.y - p.y) <= radius.y &&
            fabs(center.z - p.z) <= radius.z;
    }
    void BoundingBox::Grow(const glm::vec3& p)
    {
        m_pMin = glm::min(m_pMin, p);
        m_pMax = glm::max(m_pMax, p);
    }
    void BoundingBox::Grow(const BoundingBox& b)
    {
        m_pMin = glm::min(m_pMin, b.m_pMin);
        m_pMax = glm::max(m_pMax, b.m_pMax);
    }

    BoundingBox BoundingBox::Union(const BoundingBox& box1, const BoundingBox& box2)
    {
        BoundingBox res;
        res.m_pMin = glm::min(box1.m_pMin, box2.m_pMin);
        res.m_pMax = glm::max(box1.m_pMax, box2.m_pMax);
        return res;
    }
    BoundingBox BoundingBox::Intersection(const BoundingBox& box1, const BoundingBox& box2)
    {
        return BoundingBox(glm::max(box1.m_pMin, box2.m_pMin), glm::min(box1.m_pMax, box2.m_pMax));
    }
    void BoundingBox::Intersection(const BoundingBox& box1, const BoundingBox& box2, BoundingBox& box)
    {
        box.m_pMin = glm::max(box1.m_pMin, box2.m_pMin);
        box.m_pMax = glm::min(box1.m_pMax, box2.m_pMax);
    }

    #define BBOX_INTERSECTION_EPS 0.0f

    bool BoundingBox::Intersects(const BoundingBox& box1, const BoundingBox& box2)
    {
        glm::vec3 b1c = box1.Center();
        glm::vec3 b1r = box1.Extents() * 0.5f;
        glm::vec3 b2c = box2.Center();
        glm::vec3 b2r = box2.Extents() * 0.5f;

        return (fabs(b2c.x - b1c.x) - (b1r.x + b2r.x)) <= BBOX_INTERSECTION_EPS &&
            (fabs(b2c.y - b1c.y) - (b1r.y + b2r.y)) <= BBOX_INTERSECTION_EPS &&
            (fabs(b2c.z - b1c.z) - (b1r.z + b2r.z)) <= BBOX_INTERSECTION_EPS;
    }
    bool BoundingBox::Contains(const BoundingBox& box1, const BoundingBox& box2)
    {
        return box1.Contains(box2.m_pMin) && box1.Contains(box2.m_pMax);
    }
}