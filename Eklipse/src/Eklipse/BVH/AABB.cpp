#include "precompiled.h"
#include "AABB.h"

namespace Eklipse
{
    const glm::vec3& AABB::GetMin() const
    {
        return m_min;
    }
    const glm::vec3& AABB::GetMax() const
    {
        return m_max;
    }

    glm::vec3 AABB::Center() const
    {
        EK_CORE_PROFILE();

        return (m_max + m_min) * 0.5f;
    }
    glm::vec3 AABB::Extents() const
    {
        EK_CORE_PROFILE();

        return m_max - m_min;
    }
    int AABB::MaxDim() const
    {
        EK_CORE_PROFILE();

        glm::vec3 ext = Extents();

        if (ext.x >= ext.y && ext.x >= ext.z)
            return 0;
        if (ext.y >= ext.x && ext.y >= ext.z)
            return 1;
        if (ext.z >= ext.x && ext.z >= ext.y)
            return 2;

        return 0;
    }
    float AABB::SurfaceArea() const
    {
        EK_CORE_PROFILE();

        glm::vec3 ext = Extents();
        return 2.0f * (ext.x * ext.y + ext.x * ext.z + ext.y * ext.z);
    }
    bool AABB::Contains(const glm::vec3& p) const
    {
        EK_CORE_PROFILE();

        glm::vec3 center = Center();
        glm::vec3 radius = Extents() * 0.5f;
        return std::abs(center.x - p.x) <= radius.x &&
            fabs(center.y - p.y) <= radius.y &&
            fabs(center.z - p.z) <= radius.z;
    }
    void AABB::Expand(const glm::vec3& point)
    {
        m_min = glm::min(m_min, point);
        m_max = glm::max(m_max, point);
    }
}