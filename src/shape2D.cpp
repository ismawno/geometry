#include "geo/pch.hpp"
#include "geo/shape2D.hpp"

namespace geo
{
shape2D::shape2D(const kit::transform2D &transform) : m_transform(transform), m_global_centroid(transform.position)
{
}

const kit::transform2D &shape2D::transform() const
{
    return m_transform;
}

const glm::vec2 &shape2D::centroid() const
{
    return m_global_centroid;
}

void shape2D::centroid(const glm::vec2 &centroid)
{
    translate(centroid - m_global_centroid);
}

void shape2D::update()
{
    const glm::mat3 transform = m_transform.center_scale_rotate_translate3();
    on_shape_transform_update(transform);
}

void shape2D::on_shape_transform_update(const glm::mat3 &transform)
{
    m_global_centroid = transform[2];
}

void shape2D::translate(const glm::vec2 &dpos)
{
    m_transform.position += dpos;
    if (!m_pushing_update)
        update();
}
void shape2D::rotate(const float drotation)
{
    m_transform.rotation += drotation;
    if (!m_pushing_update)
        update();
}

void shape2D::position(const glm::vec2 &position)
{
    m_transform.position = position;
    if (!m_pushing_update)
        update();
}

void shape2D::rotation(const float rotation)
{
    m_transform.rotation = rotation;
    if (!m_pushing_update)
        update();
}

void shape2D::origin(const glm::vec2 &origin)
{
    m_transform.origin = origin;
    if (!m_pushing_update)
        update();
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
    m_pushing_update = true;
}
void shape2D::end_update()
{
    m_pushing_update = false;
    update();
}

#ifdef KIT_USE_YAML_CPP
YAML::Node shape2D::encode() const
{
    YAML::Node node;
    node["Transform"] = m_transform;
    return node;
}
bool shape2D::decode(const YAML::Node &node)
{
    if (!node.IsMap() || node.size() < 1)
        return false;

    m_transform = node["Transform"].as<kit::transform2D>();
    update();
    return true;
}
#endif
} // namespace geo