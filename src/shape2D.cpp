#include "geo/pch.hpp"
#include "geo/shape2D.hpp"

namespace geo
{
shape2D::shape2D(const glm::vec2 &centroid, const float angle) : m_centroid(centroid), m_angle(angle)
{
}

void shape2D::translate(const glm::vec2 &dpos)
{
    m_centroid += dpos;
    if (!m_pushing)
        update();
}
void shape2D::centroid(const glm::vec2 &centroid)
{
    m_centroid = centroid;
    if (!m_pushing)
        update();
}
const glm::vec2 &shape2D::centroid() const
{
    return m_centroid;
}

bool shape2D::contains_origin() const
{
    return contains_point(glm::vec2(0.f));
}
const aabb2D &shape2D::bounding_box() const
{
    return m_aabb;
}

void shape2D::begin_update()
{
    m_pushing = true;
}
void shape2D::end_update()
{
    m_pushing = false;
    update();
}

void shape2D::rotate(const float dangle)
{
    m_angle += dangle;
    if (!m_pushing)
        update();
}
void shape2D::rotation(const float angle)
{
    m_angle = angle;
    if (!m_pushing)
        update();
}
float shape2D::rotation() const
{
    return m_angle;
}

#ifdef KIT_USE_YAML_CPP
YAML::Node shape2D::encode() const
{
    YAML::Node node;
    node["Centroid"] = m_centroid;
    node["Angle"] = m_angle;
    return node;
}
bool shape2D::decode(const YAML::Node &node)
{
    if (!node.IsMap() || node.size() < 2)
        return false;

    centroid(node["Centroid"].as<glm::vec2>());
    rotation(node["Angle"].as<float>()); // Should call rotation
    return true;
}
#endif
} // namespace geo