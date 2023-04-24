#include "geo/circle.hpp"
#include <glm/geometric.hpp>
#include <glm/gtx/norm.hpp>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

namespace geo
{
    circle::circle(const float radius) : m_radius(radius) {}
    circle::circle(const glm::vec2 &pos, const float radius) : shape2D(pos),
                                                               m_radius(radius) {}

    glm::vec2 circle::support_point(const glm::vec2 &direction) const
    {
        return m_centroid + glm::normalize(direction) * m_radius;
    }

    bool circle::contains_point(const glm::vec2 &p) const { return glm::length2(p - m_centroid) <= m_radius * m_radius; }
    bool circle::contains_origin() const { return contains_point(glm::vec2(0.f)); }

    float circle::area() const { return (float)M_PI * m_radius * m_radius; }
    float circle::inertia() const { return 0.5f * m_radius * m_radius; }

    float circle::radius() const { return m_radius; }
    void circle::radius(float radius) { m_radius = radius; }
}