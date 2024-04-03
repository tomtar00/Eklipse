#pragma once
#include <glm/glm.hpp>

namespace Eklipse
{
    class AABB
    {
    public:
        AABB();
        AABB(const glm::vec3& min, const glm::vec3& max) : m_min(min), m_max(max) {}

        const glm::vec3& GetMin() const;
        const glm::vec3& GetMax() const;

        glm::vec3 Center() const;
        glm::vec3 Extents() const;
        int MaxDim() const;
        float SurfaceArea() const;
        bool Contains(const glm::vec3& p) const;
        void Expand(const glm::vec3& point);
        void Expand(const AABB& aabb);

        AABB operator+(const float value) const;

    private:
        glm::vec3 m_min;
        glm::vec3 m_max;
    };
}