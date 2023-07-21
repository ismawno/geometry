#include "geo/pch.hpp"
#include "geo/circle.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

namespace geo
{
circle::circle(const float radius) : m_radius(radius)
{
}
circle::circle(const glm::vec2 &pos, const float radius) : shape2D(pos), m_radius(radius)
{
    KIT_ASSERT_WARN(radius >= 0.f, "Creating circle with negative radius: {0}", radius);
}

circle::circle(const glm::vec2 &pos, const float radius, const float angle) : shape2D(pos), m_radius(radius)
{
    rotate(angle);
}

glm::vec2 circle::support_point(const glm::vec2 &direction) const
{
    return m_centroid + glm::normalize(direction) * m_radius;
}

bool circle::contains_point(const glm::vec2 &p) const
{
    return glm::length2(p - m_centroid) < m_radius * m_radius;
}

float circle::area() const
{
    return (float)M_PI * m_radius * m_radius;
}
float circle::inertia() const
{
    return 0.5f * m_radius * m_radius;
}

glm::vec2 circle::closest_direction_from(const glm::vec2 &p) const
{
    const glm::vec2 dir = m_centroid - p;
    return dir - glm::normalize(dir) * m_radius;
}

void circle::update()
{
    m_aabb.bound(*this);
}

float circle::radius() const
{
    return m_radius;
}
void circle::radius(float radius)
{
    m_radius = radius;
}
bool circle::is_convex() const
{
    return true;
}

#ifdef KIT_USE_YAML_CPP
void circle::write(YAML::Emitter &out) const
{
    shape2D::write(out);
    out << YAML::Key << "Radius" << YAML::Value << m_radius;
}
YAML::Node circle::encode() const
{
    YAML::Node node = shape2D::encode();
    node["Radius"] = m_radius;
    return node;
}
bool circle::decode(const YAML::Node &node)
{
    if (!shape2D::decode(node) || node.size() != 3)
        return false;
    m_radius = node["Radius"].as<float>();
    return true;
}
#endif
} // namespace geo

#ifdef KIT_USE_YAML_CPP
namespace YAML
{
Node convert<geo::circle>::encode(const geo::circle &c)
{
    return c.encode();
}
bool convert<geo::circle>::decode(const Node &node, geo::circle &c)
{
    return c.decode(node);
};
} // namespace YAML
#endif