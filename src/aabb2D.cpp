#include <limits>
#include "aabb2D.hpp"

namespace geo
{
    aabb2D::aabb2D(const alg::vec2 &point) : aabb2D(point, point) {}
    aabb2D::aabb2D(const alg::vec2 &min, const alg::vec2 &max) : m_min(min), m_max(max) {}

    void aabb2D::bound(const std::vector<alg::vec2> &vertices)
    {
        m_min.x = m_min.y = std::numeric_limits<float>::max();
        m_max.x = m_max.y = -std::numeric_limits<float>::max();
        for (const alg::vec2 &v : vertices)
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
    }

    bool aabb2D::overlaps(const aabb2D &box) const
    {
        const alg::vec2 df1 = box.m_min - m_max,
                        df2 = m_min - box.m_max;
        if (df1.x > 0.f || df1.y > 0.f)
            return false;
        if (df2.x > 0.f || df2.y > 0.f)
            return false;
        return true;
    }

    const alg::vec2 &aabb2D::min() const { return m_min; }
    const alg::vec2 &aabb2D::max() const { return m_max; }
}