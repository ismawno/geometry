#include <algorithm>
#include "box2D.hpp"

namespace geo
{
    box2D::box2D(const std::vector<vec2> &vertices) : m_vertices(&vertices) { bound(); }

    void box2D::bound()
    {
        const auto &[min, max] = std::minmax_element(m_vertices->begin(), m_vertices->end());
        m_min = *min;
        m_max = *max;
    }

    bool box2D::overlaps(const box2D &box) const
    {
        const vec2 df1 = box.m_min - m_min;
        const vec2 df2 = box.m_max - m_max;
        if (df1.x > 0.f || df1.y > 0.f)
            return false;
        if (df2.x > 0.f || df2.y > 0.f)
            return false;
        return true;
    }

    const vec2 &box2D::min() const { return m_min; }
    const vec2 &box2D::max() const { return m_max; }
}