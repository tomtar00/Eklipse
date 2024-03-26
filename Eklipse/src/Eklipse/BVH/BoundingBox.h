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

#include <glm/glm.hpp>

namespace Eklipse
{
    class BoundingBox
    {
        friend class BVH;
        friend class SplitBVH;
        friend class BVHTranslator;

    public:
        BoundingBox() : m_pMin(glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX)), m_pMax(glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX)) {}
        BoundingBox(glm::vec3 const& p) : m_pMin(p), m_pMax(p) {}
        BoundingBox(glm::vec3 const& p1, glm::vec3 const& p2) : m_pMin(glm::min(p1, p2)), m_pMax(glm::max(p1, p2)) {}

        glm::vec3 Center()  const;
        glm::vec3 Extents() const;

        int MaxDim() const;
        float SurfaceArea() const;
        bool Contains(const glm::vec3& p) const;

        void Grow(const glm::vec3& p);
        void Grow(const BoundingBox& b);

        glm::vec3 const& operator[](int i) const { return *(&m_pMin + i); }

    private:
        glm::vec3 m_pMin;
        glm::vec3 m_pMax;

    public:
        static BoundingBox Union(const BoundingBox& box1, const BoundingBox& box2);
        static BoundingBox Intersection(const BoundingBox& box1, const BoundingBox& box2);
        static void Intersection(const BoundingBox& box1, const BoundingBox& box2, BoundingBox& box);
        static bool Intersects(const BoundingBox& box1, const BoundingBox& box2);
        static bool Contains(const BoundingBox& box1, const BoundingBox& box2);
    };

    
}