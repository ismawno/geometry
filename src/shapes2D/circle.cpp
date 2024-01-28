#include "geo/internal/pch.hpp"
#include "geo/shapes2D/circle.hpp"
#include "geo/serialization/serialization.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

namespace geo
{
circle::circle(const float radius) : m_radius(radius)
{
    KIT_ASSERT_WARN(radius >= 0.f, "Creating circle with negative radius: {0}", radius);
    m_convex = true;
    update_area_and_inertia();
    update();
}
circle::circle(const kit::transform2D<float> &transform, const float radius) : shape2D(transform), m_radius(radius)
{
    KIT_ASSERT_WARN(radius >= 0.f, "Creating circle with negative radius: {0}", radius);
    m_convex = true;
    update_area_and_inertia();
    update();
}

float circle::radius() const
{
    return m_radius;
}
void circle::radius(const float radius)
{
    KIT_ASSERT_WARN(radius >= 0.f, "Setting circle radius to negative value: {0}", radius);
    m_radius = radius;
    update_area_and_inertia();
}

glm::vec2 circle::support_point(const glm::vec2 &direction) const
{
    return m_transform.position + glm::normalize(direction) * m_radius;
}

bool circle::contains_point(const glm::vec2 &p) const
{
    return glm::length2(p - m_transform.position) < m_radius * m_radius;
}

void circle::update_area_and_inertia()
{
    const float r2 = m_radius * m_radius;
    m_area = (float)M_PI * r2;
    m_inertia = 0.5f * r2;
}

void circle::bound()
{
    const glm::vec2 r = glm::vec2(m_radius);
    m_aabb.min = m_centroid - r;
    m_aabb.max = m_centroid + r;
}

glm::vec2 circle::closest_direction_from(const glm::vec2 &p) const
{
    const glm::vec2 dir = m_transform.position - p;
    return dir - glm::normalize(dir) * m_radius;
}

#ifdef KIT_USE_YAML_CPP
YAML::Node circle::encode() const
{
    return kit::yaml::codec<circle>::encode(*this);
}
bool circle::decode(const YAML::Node &node)
{
    return kit::yaml::codec<circle>::decode(node, *this);
}
#endif

} // namespace geo
