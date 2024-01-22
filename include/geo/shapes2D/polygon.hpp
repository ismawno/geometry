#pragma once

#include "geo/shapes2D/shape2D.hpp"
#include "geo/serialization/serialization.hpp"
#include "geo/shapes2D/vertices2D.hpp"
#include "kit/utility/utils.hpp"
#include <vector>
#include <array>
#include <utility>

namespace geo
{
template <std::size_t Capacity = SIZE_MAX> class polygon final : public shape2D
{
  public:
    template <kit::Iterator<glm::vec2> It> polygon(It it1, It it2) : m_vertices(it1, it2)
    {
        m_transform.position = initialize_properties_and_local_vertices();
        update();
    }
    template <std::size_t Size>
    polygon(const std::array<glm::vec2, Size> &vertices) : polygon(vertices.begin(), vertices.end())
    {
    }
    template <std::size_t Size>
    polygon(const kit::dynarray<glm::vec2, Size> &vertices = square(1.f)) : polygon(vertices.begin(), vertices.end())
    {
    }
    polygon(std::initializer_list<glm::vec2> vertices) : m_vertices(vertices)
    {
        m_transform.position = initialize_properties_and_local_vertices();
        update();
    }
    polygon(const std::vector<glm::vec2> &vertices) : m_vertices(vertices)
    {
        m_transform.position = initialize_properties_and_local_vertices();
        update();
    }
    polygon(std::vector<glm::vec2> &&vertices) : m_vertices(std::move(vertices))
    {
        m_transform.position = initialize_properties_and_local_vertices();
        update();
    }

    template <kit::Iterator<glm::vec2> It>
    polygon(const kit::transform2D<float> &transform, It it1, It it2) : shape2D(transform), m_vertices(it1, it2)
    {
        initialize_properties_and_local_vertices();
        update();
    }
    template <std::size_t Size>
    polygon(const kit::transform2D<float> &transform, const std::array<glm::vec2, Size> &vertices)
        : polygon(transform, vertices.begin(), vertices.end())
    {
    }
    template <std::size_t Size>
    polygon(const kit::transform2D<float> &transform, const kit::dynarray<glm::vec2, Size> &vertices = square(1.f))
        : polygon(transform, vertices.begin(), vertices.end())
    {
    }
    polygon(const kit::transform2D<float> &transform, std::initializer_list<glm::vec2> vertices) : m_vertices(vertices)
    {
        initialize_properties_and_local_vertices();
        update();
    }
    polygon(const kit::transform2D<float> &transform, const std::vector<glm::vec2> &vertices) : m_vertices(vertices)
    {
        initialize_properties_and_local_vertices();
        update();
    }
    polygon(const kit::transform2D<float> &transform, std::vector<glm::vec2> &&vertices)
        : m_vertices(std::move(vertices))
    {
        initialize_properties_and_local_vertices();
        update();
    }

    glm::vec2 support_point(const glm::vec2 &direction) const override
    {
        std::size_t support = 0;
        float max_dot = glm::dot(direction, m_vertices.globals[support] - m_global_centroid);
        for (std::size_t i = 1; i < m_vertices.size(); i++)
        {
            const float dot = glm::dot(direction, m_vertices.globals[i] - m_global_centroid);
            if (dot > max_dot)
            {
                max_dot = dot;
                support = i;
            }
        }
        return m_vertices.globals[support];
    }

    bool is_convex() const override
    {
        for (std::size_t i = 0; i < m_vertices.size(); i++)
            if (kit::cross2D(m_vertices.edges[i], m_vertices.edges[i + 1]) < 0.f)
                return false;

        return true;
    }
    bool contains_point(const glm::vec2 &p) const override
    {
        KIT_ASSERT_WARN(is_convex(),
                        "Checking if a point is contained in a non convex polygon yields undefined behaviour.")
        for (std::size_t i = 0; i < m_vertices.size(); i++)
        {
            const glm::vec2 &normal = m_vertices.normals[i];
            const glm::vec2 side = p - m_vertices.globals[i];
            if (glm::dot(normal, side) > 0.f)
                return false;
        }
        return true;
    }

    glm::vec2 closest_direction_from(const glm::vec2 &p) const override
    {
        float min_dist = FLT_MAX;
        glm::vec2 closest(0.f);
        for (std::size_t i = 0; i < m_vertices.size(); i++)
        {
            const glm::vec2 towards = towards_segment_from(m_vertices.globals[i], m_vertices.globals[i + 1], p);
            const float dist = glm::length2(towards);
            if (min_dist > dist)
            {
                min_dist = dist;
                closest = towards;
            }
        }
        return closest;
    }

    void bound() override
    {
        m_aabb.min = glm::vec2(FLT_MAX);
        m_aabb.max = -glm::vec2(FLT_MAX);
        for (std::size_t i = 0; i < m_vertices.size(); i++)
        {
            const glm::vec2 &v = m_vertices.globals[i];
            if (m_aabb.min.x > v.x)
                m_aabb.min.x = v.x;
            if (m_aabb.min.y > v.y)
                m_aabb.min.y = v.y;
            if (m_aabb.max.x < v.x)
                m_aabb.max.x = v.x;
            if (m_aabb.max.y < v.y)
                m_aabb.max.y = v.y;
        }
    }

    static kit::dynarray<glm::vec2, 4> square(const float size)
    {
        const float hsize = 0.5f * size;
        return kit::dynarray<glm::vec2, 4>{
            {glm::vec2(-hsize, -hsize), glm::vec2(hsize, -hsize), glm::vec2(hsize, hsize), glm::vec2(-hsize, hsize)}};
    }
    static kit::dynarray<glm::vec2, 4> rect(const float width, const float height)
    {
        const float hw = 0.5f * width;
        const float hh = 0.5f * height;
        return kit::dynarray<glm::vec2, 4>{
            {glm::vec2(-hw, -hh), glm::vec2(hw, -hh), glm::vec2(hw, hh), glm::vec2(-hw, hh)}};
    }

    template <std::size_t MaxEdges = Capacity>
    static kit::dynarray<glm::vec2, MaxEdges> ngon(const float radius, const std::uint32_t edges)
    {
        KIT_ASSERT_ERROR(edges >= 3, "Cannot make polygon with less than 3 edges - edges: {0}", edges)

        if constexpr (MaxEdges == SIZE_MAX)
        {
            std::vector<glm::vec2> vertices;
            vertices.reserve(edges);
            const float dangle = 2.f * (float)M_PI / edges;
            for (std::size_t i = 0; i < edges; i++)
            {
                const float rotation = i * dangle;
                vertices.emplace_back(radius * sinf(rotation), radius * cosf(rotation));
            }
            return vertices;
        }
        else
        {
            KIT_ASSERT_ERROR(edges <= MaxEdges, "Cannot make polygon with more than {0} edges - edges: {1}", MaxEdges,
                             edges)
            kit::dynarray<glm::vec2, MaxEdges> vertices{edges};
            const float dangle = 2.f * (float)M_PI / edges;
            for (std::size_t i = 0; i < edges; i++)
            {
                const float rotation = i * dangle;
                vertices[i] = {radius * sinf(rotation), radius * cosf(rotation)};
            }
            return vertices;
        }
    }

    float area() const override
    {
        return m_area;
    }
    float inertia() const override
    {
        return m_inertia;
    }

    const vertices2D<Capacity> &vertices() const
    {
        return m_vertices;
    }

#ifdef KIT_USE_YAML_CPP
    YAML::Node encode() const override
    {
        return kit::yaml::codec<polygon>::encode(*this);
    }
    bool decode(const YAML::Node &node) override
    {
        return kit::yaml::codec<polygon>::decode(node, *this);
    }
#endif

  private:
    vertices2D<Capacity> m_vertices;
    float m_area = 0.f;
    float m_inertia = 0.f;

    void on_shape_transform_update(const glm::mat3 &transform) override
    {
        shape2D::on_shape_transform_update(transform);
        for (std::size_t i = 0; i < m_vertices.size(); i++)
            m_vertices.globals[i] = transform * glm::vec3(m_vertices.locals[i], 1.f);
        for (std::size_t i = 0; i < m_vertices.size(); i++)
        {
            m_vertices.edges[i] = m_vertices.globals[i + 1] - m_vertices.globals[i];
            m_vertices.normals[i] = glm::normalize(glm::vec2(m_vertices.edges[i].y, -m_vertices.edges[i].x));
        }
    }

    void sort_global_vertices()
    {
        const glm::vec2 center =
            center_of_vertices(m_vertices.globals.begin(), m_vertices.globals.end(), m_vertices.size());
        const glm::vec2 reference = m_vertices.globals[0] - center;

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
        std::sort(m_vertices.globals.begin(), m_vertices.globals.end(), cmp);
    }

    glm::vec2 initialize_properties_and_local_vertices()
    {
        sort_global_vertices();
        const glm::vec2 current_centroid = compute_center_of_mass(*this);

        for (std::size_t i = 0; i < m_vertices.size(); i++)
            m_vertices.locals[i] = m_vertices.globals[i] - current_centroid;

        m_area = compute_area(*this);
        m_inertia = compute_inertia(*this);
        return current_centroid;
    }

    template <kit::Iterator<glm::vec2> It> static glm::vec2 center_of_vertices(It it1, It it2, const std::size_t size)
    {
        glm::vec2 center(0.f);
        for (It it = it1; it != it2; it++)
            center += *it;
        return center / (float)size;
    }

    static glm::vec2 compute_center_of_mass(const polygon &poly)
    {
        const glm::vec2 &p1 = poly.m_vertices.globals[0]; // No locals available yet
        glm::vec2 num(0.f), den(0.f);
        for (std::size_t i = 1; i < poly.size() - 1; i++)
        {
            const glm::vec2 e1 = poly.m_vertices.globals[i] - p1;
            const glm::vec2 e2 = poly.m_vertices.globals[i + 1] - p1;

            const float crs = std::abs(kit::cross2D(e1, e2));
            num += (e1 + e2) * crs;
            den += crs;
        }
        return p1 + num / (3.f * den);
    }

    static float compute_area(const polygon &poly)
    {
        float area = 0.f;
        const glm::vec2 &p1 = poly.m_vertices.locals[0];

        for (std::size_t i = 1; i < poly.size() - 1; i++)
        {
            const glm::vec2 e1 = poly.m_vertices.locals[i] - p1;
            const glm::vec2 e2 = poly.m_vertices.locals[i + 1] - p1;
            area += std::abs(kit::cross2D(e1, e2));
        }
        return area * 0.5f;
    }

    static float compute_inertia(const polygon &poly)
    {
        const glm::vec2 &p1 = poly.m_vertices.locals[0];
        float inertia = 0.f;

        for (std::size_t i = 1; i < poly.size() - 1; i++)
        {
            const glm::vec2 &p2 = poly.m_vertices.locals[i];
            const glm::vec2 &p3 = poly.m_vertices.locals[i + 1];
            const glm::vec2 e1 = p1 - p2;
            const glm::vec2 e2 = p3 - p2;

            const float w = glm::length(e1), w1 = std::abs(glm::dot(e1, e2) / w), w2 = std::abs(w - w1);

            const float h = std::abs(kit::cross2D(e2, e1)) / w;
            const glm::vec2 p4 = p2 + e1 * w1 / w;

            const float i1 = w1 * h * (w1 * w1 / 3.f + h * h) / 4.f, i2 = w2 * h * (w2 * w2 / 3.f + h * h) / 4.f;
            const float m1 = 0.5f * w1 * h, m2 = 0.5f * w2 * h;

            const glm::vec2 cm1 = (p2 + p3 + p4) / 3.f, cm2 = (p1 + p3 + p4) / 3.f;

            const float icm1 = i1 + m1 * (glm::length2(cm1) - glm::distance2(cm1, p3)),
                        icm2 = i2 + m2 * (glm::length2(cm2) - glm::distance2(cm2, p3));

            const glm::vec2 p13 = p1 - p3, p43 = p4 - p3, p23 = -e2;
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

    static glm::vec2 towards_segment_from(const glm::vec2 &p1, const glm::vec2 &p2, const glm::vec2 &p)
    {
        const float interp = std::clamp(glm::dot(p - p1, p2 - p1) / glm::distance2(p1, p2), 0.f, 1.f);
        const glm::vec2 proj = p1 + interp * (p2 - p1);
        return proj - p;
    }
};
} // namespace geo
