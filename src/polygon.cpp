#include "geo/polygon.hpp"
#include "debug/debug.hpp"
#include <glm/geometric.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <algorithm>
#include <cstdint>
#include <cmath>
#include <limits>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

namespace geo
{
    static float cross(const glm::vec2 &v1, const glm::vec2 &v2) { return v1.x * v2.y - v1.y * v2.x; }

    polygon::polygon(const std::vector<glm::vec2> &vertices) : m_vertices(vertices)
    {
        DBG_ASSERT(m_vertices.size() >= 3, "Cannot make polygon with less than 3 vertices - vertices: %zu\n", m_vertices.size())
        sort_vertices();
        m_centroid = centre_of_mass(*this);
        m_area = area(*this);
        m_inertia = inertia(*this);
    }

    polygon::polygon(const glm::vec2 &pos,
                     const std::vector<glm::vec2> &vertices) : polygon(vertices)
    {
        this->pos(pos);
    }

    glm::vec2 polygon::centre_of_vertices(const polygon &poly)
    {
        glm::vec2 centre(0.f);
        for (const glm::vec2 &v : poly.vertices())
            centre += v;
        return centre / (float)poly.size();
    }

    glm::vec2 polygon::centre_of_mass(const polygon &poly)
    {
        const glm::vec2 &p1 = poly[0];
        glm::vec2 num(0.f), den(0.f);
        for (std::size_t i = 1; i < poly.size() - 1; i++)
        {
            const glm::vec2 e1 = poly[i] - p1, e2 = poly[i + 1] - p1;
            const float crs = std::abs(cross(e1, e2));
            num += (e1 + e2) * crs;
            den += crs;
        }
        return p1 + num / (3.f * den);
    }

    float polygon::area(const polygon &poly)
    {
        float area = 0.f;
        const glm::vec2 &p1 = poly[0];
        for (std::size_t i = 1; i < poly.size() - 1; i++)
        {
            const glm::vec2 e1 = poly[i] - p1, e2 = poly[i + 1] - p1;
            area += std::abs(cross(e1, e2));
        }
        return area * 0.5f;
    }

    float polygon::inertia(const polygon &poly)
    {
        const glm::vec2 &p1 = poly[0];
        float inertia = 0.f;
        for (std::size_t i = 1; i < poly.size() - 1; i++)
        {
            const glm::vec2 &p2 = poly[i], &p3 = poly[i + 1];
            const glm::vec2 e1 = p1 - p2, e2 = p3 - p2;

            const float w = glm::length(e1),
                        w1 = std::abs(glm::dot(e1, e2) / w),
                        w2 = std::abs(w - w1);

            const float h = std::abs(cross(e2, e1)) / w;
            const glm::vec2 p4 = p2 + (p1 - p2) * w1 / w;

            const float i1 = w1 * h * (w1 * w1 / 3.f + h * h) / 4.f,
                        i2 = w2 * h * (w2 * w2 / 3.f + h * h) / 4.f;
            const float m1 = 0.5f * w1 * h, m2 = 0.5f * w2 * h;

            const glm::vec2 cm1 = (p2 + p3 + p4) / 3.f, cm2 = (p1 + p3 + p4) / 3.f;

            const float icm1 = i1 + m1 * (glm::distance2(cm1, poly.centroid()) - glm::distance2(cm1, p3)),
                        icm2 = i2 + m2 * (glm::distance2(cm2, poly.centroid()) - glm::distance2(cm2, p3));

            const glm::vec2 p13 = p1 - p3, p43 = p4 - p3, p23 = p2 - p3;
            if (cross(p13, p43) < 0.f)
                inertia += icm1;
            else
                inertia -= icm1;
            if (cross(p43, p23) < 0.f)
                inertia += icm2;
            else
                inertia -= icm2;
        }
        return std::abs(inertia) / poly.area();
    }

    void polygon::translate(const glm::vec2 &dpos)
    {
        for (glm::vec2 &v : m_vertices)
            v += dpos;
        m_centroid += dpos;
    }

    glm::vec2 polygon::support_point(const glm::vec2 &direction) const
    {
        const auto cmp = [&direction, this](const glm::vec2 &v1, const glm::vec2 &v2)
        { return glm::dot(direction, v1 - m_centroid) < glm::dot(direction, v2 - m_centroid); };

        const auto &support = std::max_element(m_vertices.begin(), m_vertices.end(), cmp);
        return *support;
    }

    bool polygon::is_convex() const
    {
        for (std::size_t i = 0; i < m_vertices.size(); i++)
        {
            const glm::vec2 &prev = (*this)[i],
                            &mid = (*this)[i + 1],
                            &next = (*this)[i + 2];
            if (cross(mid - prev, next - mid) < 0.f)
                return false;
        }
        return true;
    }

    bool polygon::contains_point(const glm::vec2 &p) const
    {
        DBG_LOG_IF(!is_convex(), "Checking if a point is contained in a non convex polygon yields undefined behaviour.\n")
        for (std::size_t i = 0; i < m_vertices.size(); i++)
        {
            const glm::vec2 &v1 = (*this)[i], &v2 = (*this)[i + 1];
            if (line_intersects_edge(v2, v1, p, m_centroid) && line_intersects_edge(p, m_centroid, v2, v1))
                return false;
        }
        return true;
    }

    bool polygon::contains_origin() const { return contains_point(glm::vec2(0.f)); }

    glm::vec2 polygon::towards_segment_from(const glm::vec2 &p1,
                                            const glm::vec2 &p2,
                                            const glm::vec2 &p)
    {
        const float t = std::clamp(glm::dot(p - p1, p2 - p1) / glm::distance2(p1, p2), 0.f, 1.f);
        const glm::vec2 proj = p1 + t * (p2 - p1);
        return proj - p;
    }

    glm::vec2 polygon::closest_direction_from(const glm::vec2 &p) const
    {
        float min_dist = std::numeric_limits<float>::max();
        glm::vec2 closest(0.f);
        for (std::size_t i = 0; i < m_vertices.size(); i++)
        {
            const glm::vec2 towards = towards_segment_from((*this)[i], (*this)[i + 1], p);
            const float dist = glm::length2(towards);
            if (min_dist > dist)
            {
                min_dist = dist;
                closest = towards;
            }
        }
        return closest;
    }

    bool polygon::line_intersects_edge(const glm::vec2 &l1,
                                       const glm::vec2 &l2,
                                       const glm::vec2 &v1,
                                       const glm::vec2 &v2)
    {
        const float a = l2.y - l1.y, b = l1.x - l2.x;
        const float c = l2.x * l1.y - l1.x * l2.y;

        const float d1 = a * v1.x + b * v1.y + c;
        const float d2 = a * v2.x + b * v2.y + c;
        return !((d1 > 0.f && d2 > 0.f) || (d1 < 0.f && d2 < 0.f));
    }

    void polygon::sort_vertices()
    {
        const glm::vec2 centre = centre_of_vertices(*this),
                        ref = m_vertices[0] - centre;
        const auto cmp = [&centre, &ref](const glm::vec2 &v1, const glm::vec2 &v2)
        {
            const glm::vec2 dir1 = v1 - centre, dir2 = v2 - centre;

            const double det2 = cross(ref, dir2);
            if (det2 == 0.f && glm::dot(ref, dir2) >= 0.f)
                return false;
            const double det1 = cross(ref, dir1);
            if (det1 == 0.f && glm::dot(ref, dir1) >= 0.f)
                return true;

            if (det1 * det2 >= 0.f)
                return cross(dir1, dir2) > 0.f;
            return det1 > 0.f;
        };
        std::sort(m_vertices.begin(), m_vertices.end(), cmp);
    }

    void polygon::write(ini::output &out) const
    {
        std::size_t index = 0;
        const std::string key = "vertex";

        for (const glm::vec2 &v : m_vertices)
        {
            out.write(key + std::to_string(index) + "x", v.x);
            out.write(key + std::to_string(index++) + "y", v.y);
        }
    }
    void polygon::read(ini::input &in)
    {
        std::vector<glm::vec2> vertices;
        vertices.reserve(m_vertices.capacity());

        std::size_t index = 0;
        const std::string key = "vertex";
        while (true)
        {
            const std::string kx = key + std::to_string(index) + "x",
                              ky = key + std::to_string(index++) + "y";
            DBG_ASSERT((in.contains_key(kx) && in.contains_key(ky)) ||
                           (!in.contains_key(kx) && !in.contains_key(ky)),
                       "Vector key only contains a component of the vector! Weird.\n")
            if (!in.contains_key(kx) || !in.contains_key(ky)) // Just for ick reasons
                break;
            vertices.emplace_back(in.readf32(kx), in.readf32(ky));
        }
        *this = geo::polygon(vertices);
    }

    void polygon::rotate(float dangle)
    {
        for (glm::vec2 &v : m_vertices)
            v = glm::rotate(v - m_centroid, dangle) + m_centroid;
        m_angle += dangle;
    }

    const std::vector<glm::vec2> &polygon::vertices() const { return m_vertices; }

    std::size_t polygon::size() const { return m_vertices.size(); }

    float polygon::area() const { return m_area; }
    float polygon::inertia() const { return m_inertia; }

    glm::vec2 polygon::relative(const std::size_t index) const { return this->operator[](index) - m_centroid; }

    const glm::vec2 &polygon::operator[](const std::size_t index) const
    {
        return m_vertices[index % m_vertices.size()];
    }

    std::vector<glm::vec2> polygon::box(const float size)
    {
        return {
            {-size / 2.f, -size / 2.f},
            {size / 2.f, -size / 2.f},
            {size / 2.f, size / 2.f},
            {-size / 2.f, size / 2.f}};
    }
    std::vector<glm::vec2> polygon::rect(const float width, const float height)
    {
        return {
            {-width / 2.f, -height / 2.f},
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
            const float angle = i * dangle;
            vertices.emplace_back(glm::vec2(radius * sinf(angle), radius * cosf(angle)));
        }
        return vertices;
    }

    polygon polygon::minkowski_sum(const polygon &poly1, const polygon &poly2)
    {
        std::vector<glm::vec2> sum;
        sum.reserve(poly1.size() + poly2.size());

        const auto cmp = [](const glm::vec2 &v1, const glm::vec2 &v2)
        { return v1.x < v2.x; };
        const std::size_t m1 = (std::size_t)std::distance(poly1.vertices().begin(),
                                                          std::min_element(poly1.vertices().begin(),
                                                                           poly1.vertices().end(), cmp)),

                          m2 = (std::size_t)std::distance(poly2.vertices().begin(),
                                                          std::min_element(poly2.vertices().begin(),
                                                                           poly2.vertices().end(), cmp));

        std::size_t i = 0, j = 0;
        while (i < poly1.size() || j < poly2.size())
        {
            const std::size_t index1 = i + m1, index2 = j + m2;
            sum.emplace_back(poly1[index1] + poly2[index2]);
            const float crs = cross(poly1[index1 + 1] - poly1[index1],
                                    poly2[index2 + 1] - poly2[index2]);
            if (crs >= 0.f)
                i++;
            if (crs <= 0.f)
                j++;
        }
        return polygon(sum);
    }

    polygon polygon::minkowski_difference(const polygon &poly1, const polygon &poly2)
    {
        return minkowski_sum(poly1, -poly2);
    }

    polygon operator-(const polygon &poly)
    {
        std::vector<glm::vec2> vertices;
        vertices.reserve(poly.size());
        for (const glm::vec2 &v : poly.vertices())
            vertices.emplace_back(-v);
        return polygon(vertices);
    }

    polygon operator+(const polygon &poly1, const polygon &poly2)
    {
        return polygon::minkowski_sum(poly1, poly2);
    }

    polygon operator-(const polygon &poly1, const polygon &poly2)
    {
        return polygon::minkowski_difference(poly1, poly2);
    }
}