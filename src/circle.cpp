#include "geo/pch.hpp"
#include "geo/circle.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

namespace geo
{
circle::circle(const float radius) : radius(radius)
{
    KIT_ASSERT_WARN(radius >= 0.f, "Creating circle with negative radius: {0}", radius);
}
circle::circle(const kit::transform2D &transform, const float radius) : shape2D(transform), radius(radius)
{
    KIT_ASSERT_WARN(radius >= 0.f, "Creating circle with negative radius: {0}", radius);
}

glm::vec2 circle::support_point(const glm::vec2 &direction) const
{
    return m_transform.position + glm::normalize(direction) * radius;
}

bool circle::contains_point(const glm::vec2 &p) const
{
    return glm::length2(p - m_transform.position) < radius * radius;
}

float circle::area() const
{
    return (float)M_PI * radius * radius;
}
float circle::inertia() const
{
    return 0.5f * radius * radius;
}

glm::vec2 circle::closest_direction_from(const glm::vec2 &p) const
{
    const glm::vec2 dir = m_transform.position - p;
    return dir - glm::normalize(dir) * radius;
}

void circle::on_shape_transform_update(const glm::mat3 &transform)
{
    shape2D::on_shape_transform_update(transform);
    m_aabb.bound(*this);
}

bool circle::is_convex() const
{
    return true;
}

#ifdef KIT_USE_YAML_CPP
YAML::Node circle::encode() const
{
    YAML::Node node = shape2D::encode();
    node["Radius"] = radius;
    return node;
}
bool circle::decode(const YAML::Node &node)
{
    if (!shape2D::decode(node) || node.size() != 2)
        return false;
    radius = node["Radius"].as<float>();
    return true;
}
#endif
} // namespace geo
