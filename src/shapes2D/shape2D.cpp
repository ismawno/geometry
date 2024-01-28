#include "geo/internal/pch.hpp"
#include "geo/shapes2D/shape2D.hpp"

namespace geo
{
shape2D::shape2D(const kit::transform2D<float> &transform) : m_transform(transform), m_centroid(transform.position)
{
}

const kit::transform2D<float> &shape2D::transform() const
{
    return m_transform;
}

const glm::vec2 &shape2D::centroid() const
{
    return m_centroid;
}

float shape2D::area() const
{
    return m_area;
}
float shape2D::inertia() const
{
    return m_inertia;
}
bool shape2D::convex() const
{
    return m_convex;
}

void shape2D::centroid(const glm::vec2 &centroid)
{
    translate(centroid - m_centroid);
}

void shape2D::update()
{
    if (m_pushing_update)
        return;
    const glm::mat3 transform = m_transform.center_scale_rotate_translate3();
    on_shape_transform_update(transform);
    bound();
}

void shape2D::on_shape_transform_update(const glm::mat3 &transform)
{
    m_centroid = transform[2];
}

void shape2D::translate(const glm::vec2 &dpos)
{
    m_transform.position += dpos;
    update();
}
void shape2D::rotate(const float drotation)
{
    m_transform.rotation += drotation;
    update();
}

void shape2D::position(const glm::vec2 &position)
{
    m_transform.position = position;
    update();
}

void shape2D::rotation(const float rotation)
{
    m_transform.rotation = rotation;
    update();
}

void shape2D::origin(const glm::vec2 &origin)
{
    m_transform.origin = origin;
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

bool shape2D::updating() const
{
    return m_pushing_update;
}

void shape2D::begin_update()
{
    KIT_ASSERT_ERROR(!m_pushing_update, "Cannot begin update while already updating");
    m_pushing_update = true;
}
void shape2D::end_update()
{
    m_pushing_update = false;
    update();
}
} // namespace geo