#include "geo/internal/pch.hpp"
#include "geo/shapes2D/polygon.hpp"
#include "geo/serialization/serialization.hpp"
#include "kit/utility/utils.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

namespace geo
{
template <typename It> static glm::vec2 center_of_vertices(It it1, It it2, const std::size_t size)
{
    glm::vec2 center(0.f);
    for (auto it = it1; it != it2; ++it)
        center += *it;
    return center / (float)size;
}

static glm::vec2 center_of_mass(const polygon &poly)
{
    const glm::vec2 &p1 = poly.global(0); // No locals available yet
    glm::vec2 num(0.f), den(0.f);
    for (std::size_t i = 1; i < poly.size() - 1; i++)
    {
        const glm::vec2 e1 = poly.global(i) - p1;
        const glm::vec2 e2 = poly.global(i + 1) - p1;

        const float crs = std::abs(kit::cross2D(e1, e2));
        num += (e1 + e2) * crs;
        den += crs;
    }
    return p1 + num / (3.f * den);
}

static float area(const polygon &poly)
{
    float area = 0.f;
    const glm::vec2 &p1 = poly.local(0);
    for (std::size_t i = 1; i < poly.size() - 1; i++)
    {
        const glm::vec2 e1 = poly.local(i) - p1, e2 = poly.local(i + 1) - p1;
        area += std::abs(kit::cross2D(e1, e2));
    }
    return area * 0.5f;
}

static float inertia(const polygon &poly)
{
    const glm::vec2 &p1 = poly.local(0);
    float inertia = 0.f;
    for (std::size_t i = 1; i < poly.size() - 1; i++)
    {
        const glm::vec2 &p2 = poly.local(i), &p3 = poly.local(i + 1);
        const glm::vec2 e1 = p1 - p2, e2 = p3 - p2;

        const float w = glm::length(e1), w1 = std::abs(glm::dot(e1, e2) / w), w2 = std::abs(w - w1);

        const float h = std::abs(kit::cross2D(e2, e1)) / w;
        const glm::vec2 p4 = p2 + (p1 - p2) * w1 / w;

        const float i1 = w1 * h * (w1 * w1 / 3.f + h * h) / 4.f, i2 = w2 * h * (w2 * w2 / 3.f + h * h) / 4.f;
        const float m1 = 0.5f * w1 * h, m2 = 0.5f * w2 * h;

        const glm::vec2 cm1 = (p2 + p3 + p4) / 3.f, cm2 = (p1 + p3 + p4) / 3.f;

        const float icm1 = i1 + m1 * (glm::length2(cm1) - glm::distance2(cm1, p3)),
                    icm2 = i2 + m2 * (glm::length2(cm2) - glm::distance2(cm2, p3));

        const glm::vec2 p13 = p1 - p3, p43 = p4 - p3, p23 = p2 - p3;
        if (kit::cross2D(p13, p43) < 0.f)
            inertia += icm1;
        else
            inertia -= icm1;
        if (kit::cross2D(p43, p23) < 0.f)
            inertia += icm2;
        else
            inertia -= icm2;
    }
    return std::abs(inertia) / poly.area();
}

polygon::polygon(const std::vector<glm::vec2> &vertices) : m_vertices(vertices), m_size(vertices.size())
{
    m_transform.position = initialize_properties_and_local_vertices();
    update();
}

polygon::polygon(const kit::transform2D<float> &transform, const std::vector<glm::vec2> &vertices)
    : shape2D(transform), m_vertices(vertices), m_size(vertices.size())
{
    initialize_properties_and_local_vertices();
    update();
}

glm::vec2 polygon::initialize_properties_and_local_vertices()
{
    KIT_ASSERT_ERROR(m_size >= 3, "Cannot make polygon with less than 3 vertices - vertices: {0}", m_size)
    m_vertices.resize(6 * m_size);

    sort_global_vertices();
    const glm::vec2 current_centroid = ::geo::center_of_mass(*this);

    for (std::size_t i = 0; i < m_size; i++)
        m_vertices[i + m_size] = m_vertices[i] - current_centroid;
    for (std::size_t i = 0; i < m_size; i++)
    {
        m_vertices[i + 3 * m_size] = local(i + 1) - local(i);
        m_vertices[i + 5 * m_size] =
            glm::normalize(glm::vec2(m_vertices[i + 3 * m_size].y, -m_vertices[i + 3 * m_size].x));
    }

    m_area = ::geo::area(*this);
    m_inertia = ::geo::inertia(*this);
    return current_centroid;
}

glm::vec2 polygon::support_point(const glm::vec2 &direction) const
{
    std::size_t support = 0;
    float max_dot = glm::dot(direction, m_vertices[support] - m_global_centroid);
    for (std::size_t i = 1; i < m_size; i++)
    {
        const float dot = glm::dot(direction, m_vertices[i] - m_global_centroid);
        if (dot > max_dot)
        {
            max_dot = dot;
            support = i;
        }
    }
    return m_vertices[support];
}

bool polygon::is_convex() const
{
    for (std::size_t i = 0; i < m_size; i++)
        if (kit::cross2D(local_edge(i), local_edge(i + 1)) < 0.f)
            return false;

    return true;
}

bool polygon::contains_point(const glm::vec2 &p) const
{
    KIT_ASSERT_WARN(is_convex(), "Checking if a point is contained in a non convex polygon yields undefined behaviour.")
    for (std::size_t i = 0; i < m_size; i++)
    {
        const glm::vec2 &normal = global_edge_normal(i);
        const glm::vec2 side = p - global(i);
        if (glm::dot(normal, side) > 0.f)
            return false;
    }
    return true;
}

static glm::vec2 towards_segment_from(const glm::vec2 &p1, const glm::vec2 &p2, const glm::vec2 &p)
{
    const float interp = std::clamp(glm::dot(p - p1, p2 - p1) / glm::distance2(p1, p2), 0.f, 1.f);
    const glm::vec2 proj = p1 + interp * (p2 - p1);
    return proj - p;
}

glm::vec2 polygon::closest_direction_from(const glm::vec2 &p) const
{
    float min_dist = FLT_MAX;
    glm::vec2 closest(0.f);
    for (std::size_t i = 0; i < m_size; i++)
    {
        const glm::vec2 towards = towards_segment_from(global(i), global(i + 1), p);
        const float dist = glm::length2(towards);
        if (min_dist > dist)
        {
            min_dist = dist;
            closest = towards;
        }
    }
    return closest;
}

void polygon::sort_global_vertices()
{
    const glm::vec2 center = center_of_vertices(m_vertices.begin(), m_vertices.begin() + m_size, m_size);
    const glm::vec2 reference = m_vertices[0] - center;

    const auto cmp = [&center, &reference](const glm::vec2 &v1, const glm::vec2 &v2) {
        const glm::vec2 dir1 = v1 - center, dir2 = v2 - center;

        const float det2 = kit::cross2D(reference, dir2);
        if (kit::approaches_zero(det2) && glm::dot(reference, dir2) >= 0.f)
            return false;
        const float det1 = kit::cross2D(reference, dir1);
        if (kit::approaches_zero(det1) && glm::dot(reference, dir1) >= 0.f)
            return true;

        if (det1 * det2 >= 0.f)
            return kit::cross2D(dir1, dir2) > 0.f;
        return det1 > 0.f;
    };
    std::sort(m_vertices.begin(), m_vertices.begin() + m_size, cmp);
}

void polygon::on_shape_transform_update(const glm::mat3 &transform)
{
    shape2D::on_shape_transform_update(transform);
    for (std::size_t i = 0; i < m_size; i++)
        m_vertices[i] = transform * glm::vec3(m_vertices[i + m_size], 1.f);
    for (std::size_t i = 0; i < m_size; i++)
    {
        m_vertices[i + 2 * m_size] = global(i + 1) - global(i);
        m_vertices[i + 4 * m_size] =
            glm::normalize(glm::vec2(m_vertices[i + 2 * m_size].y, -m_vertices[i + 2 * m_size].x));
    }
    m_aabb.bound(*this);
}

polygon::vertex_subview polygon::globals() const
{
    return vertex_subview{m_vertices.begin(), m_vertices.begin() + m_size};
}
polygon::vertex_subview polygon::locals() const
{
    return vertex_subview{m_vertices.begin() + m_size, m_vertices.begin() + 2 * m_size};
}

polygon::vertex_subview polygon::global_edges() const
{
    return vertex_subview{m_vertices.begin() + 2 * m_size, m_vertices.begin() + 3 * m_size};
}
polygon::vertex_subview polygon::local_edges() const
{
    return vertex_subview{m_vertices.begin() + 3 * m_size, m_vertices.begin() + 4 * m_size};
}

polygon::vertex_subview polygon::global_edge_normals() const
{
    return vertex_subview{m_vertices.begin() + 4 * m_size, m_vertices.begin() + 5 * m_size};
}
polygon::vertex_subview polygon::local_edge_normals() const
{
    return vertex_subview{m_vertices.begin() + 5 * m_size, m_vertices.begin() + 6 * m_size};
}

const glm::vec2 &polygon::global(const std::size_t index) const
{
    return m_vertices[index % m_size];
}
const glm::vec2 &polygon::local(const std::size_t index) const
{
    return m_vertices[m_size + index % m_size];
}

const glm::vec2 &polygon::global_edge(std::size_t index) const
{
    return m_vertices[2 * m_size + index % m_size];
}
const glm::vec2 &polygon::local_edge(std::size_t index) const
{
    return m_vertices[3 * m_size + index % m_size];
}

const glm::vec2 &polygon::global_edge_normal(std::size_t index) const
{
    return m_vertices[4 * m_size + index % m_size];
}
const glm::vec2 &polygon::local_edge_normal(std::size_t index) const
{
    return m_vertices[5 * m_size + index % m_size];
}

std::size_t polygon::size() const
{
    return m_size;
}

float polygon::area() const
{
    return m_area;
}
float polygon::inertia() const
{
    return m_inertia;
}

std::vector<glm::vec2> polygon::square(const float size)
{
    return {{-size / 2.f, -size / 2.f}, {size / 2.f, -size / 2.f}, {size / 2.f, size / 2.f}, {-size / 2.f, size / 2.f}};
}
std::vector<glm::vec2> polygon::rect(const float width, const float height)
{
    return {{-width / 2.f, -height / 2.f},
            {width / 2.f, -height / 2.f},
            {width / 2.f, height / 2.f},
            {-width / 2.f, height / 2.f}};
}
std::vector<glm::vec2> polygon::ngon(const float radius, const std::uint32_t sides)
{
    std::vector<glm::vec2> vertices;
    vertices.reserve(sides);

    const float dangle = 2.f * (float)M_PI / sides;
    for (std::size_t i = 0; i < sides; i++)
    {
        const float rotation = i * dangle;
        vertices.emplace_back(radius * sinf(rotation), radius * cosf(rotation));
    }
    return vertices;
}

#ifdef KIT_USE_YAML_CPP
YAML::Node polygon::encode() const
{
    return kit::yaml::codec<polygon>::encode(*this);
}
bool polygon::decode(const YAML::Node &node)
{
    return kit::yaml::codec<polygon>::decode(node, *this);
}
#endif

} // namespace geo
