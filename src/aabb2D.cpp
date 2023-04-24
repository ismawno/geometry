#include <limits>
#include "geo/aabb2D.hpp"

namespace geo
{
    aabb2D::aabb2D(const std::vector<glm::vec2> &vertices) { bound(vertices); }
    aabb2D::aabb2D(const polygon &poly) { bound(poly); }
    aabb2D::aabb2D(const circle &c) { bound(c); }

    aabb2D::aabb2D(const glm::vec2 &point) : aabb2D(point, point) {}
    aabb2D::aabb2D(const glm::vec2 &min, const glm::vec2 &max) : m_min(min), m_max(max) {}

    void aabb2D::bound(const std::vector<glm::vec2> &vertices)
    {
        m_min = glm::vec2(std::numeric_limits<float>::max());
        m_max = -glm::vec2(std::numeric_limits<float>::max());
        for (const glm::vec2 &v : vertices)
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
    void aabb2D::bound(const polygon &poly) { bound(poly.vertices()); }
    void aabb2D::bound(const circle &c)
    {
        m_min = c.centroid() - glm::vec2(c.radius());
        m_max = c.centroid() + glm::vec2(c.radius());
    }

    const glm::vec2 &aabb2D::min() const { return m_min; }
    const glm::vec2 &aabb2D::max() const { return m_max; }
}