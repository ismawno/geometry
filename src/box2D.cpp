#include <limits>
#include "box2D.hpp"

namespace geo
{
    void box2D::bound(const std::vector<vec2> &vertices, const vec2 &centroid)
    {
        m_min.x = m_min.y = std::numeric_limits<float>::max();
        m_max.x = m_max.y = -std::numeric_limits<float>::max();
        for (const vec2 &v : vertices)
        {
            if (m_min.x > v.x)
                m_min.x = v.x;
            if (m_min.y > v.y)
                m_min.y = v.y;
            if (m_max.x < v.x)
                m_max.x = v.x;
            if (m_max.y < v.y)
                m_max.y = v.y;
        }
        m_centroid = centroid;
    }

    bool box2D::overlaps(const box2D &box) const
    {
        const vec2 df1 = box.m_min - m_max;
        const vec2 df2 = box.m_min - m_max;
        if (df1.x > 0.f || df1.y > 0.f)
            return false;
        if (df2.x > 0.f || df2.y > 0.f)
            return false;
        return true;
    }

    void box2D::recentre(const vec2 &centroid)
    {
        const vec2 dpos = centroid - m_centroid;
        m_min += dpos;
        m_max += dpos;
        m_centroid = centroid;
    }

    const vec2 &box2D::min() const { return m_min; }
    const vec2 &box2D::max() const { return m_max; }
}