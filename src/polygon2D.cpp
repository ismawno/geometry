#include "polygon2D.hpp"
#include "debug.h"
#include <algorithm>
#include <cstdint>
#include <cmath>
#include <limits>

namespace geo
{
    polygon2D::polygon2D(const vec2 &pos,
                         const std::vector<vec2> &vertices) : m_vertices(vertices)
    {
        DBG_EXIT_IF(m_vertices.size() < 3, "Cannot make polygon with less than 3 vertices.\n")
        const vec2 cvert = centre_of_vertices(*this);
        translate(-cvert);
        sort_vertices_by_angle();
        m_centroid = centre_of_mass(*this);
        this->pos(pos);
        m_area = area(*this);
        m_inertia = inertia(*this);
    }
    polygon2D::polygon2D(const std::vector<vec2> &vertices) : m_vertices(vertices)
    {
        DBG_EXIT_IF(m_vertices.size() < 3, "Cannot make polygon with less than 3 vertices.\n")
        const vec2 cvert = centre_of_vertices(*this);
        translate(-cvert);
        sort_vertices_by_angle();
        m_centroid = centre_of_mass(*this);
        translate(cvert);
        m_area = area(*this);
        m_inertia = inertia(*this);
    }

    vec2 polygon2D::centre_of_vertices(const polygon2D &poly)
    {
        vec2 centre;
        for (const vec2 &v : poly.vertices())
            centre += v;
        return centre / poly.size();
    }

    vec2 polygon2D::centre_of_mass(const polygon2D &poly)
    {
        const vec2 &p1 = poly[0];
        vec2 num, den;
        for (std::size_t i = 1; i < poly.size() - 1; i++)
        {
            const vec2 e1 = poly[i] - p1, e2 = poly[i + 1] - p1;
            const float cross = std::abs(e1.cross(e2));
            num += (e1 + e2) * cross;
            den += cross;
        }
        return p1 + num / (3.f * den);
    }

    float polygon2D::area(const polygon2D &poly)
    {
        float area = 0.f;
        const vec2 &p1 = poly[0];
        for (std::size_t i = 1; i < poly.size() - 1; i++)
        {
            const vec2 e1 = poly[i] - p1, e2 = poly[i + 1] - p1;
            area += std::abs(e1.cross(e2));
        }
        return area / 2.f;
    }

    float polygon2D::inertia(const polygon2D &poly)
    {
        const vec2 &p1 = poly[0];
        float inertia = 0.f;
        for (std::size_t i = 1; i < poly.size() - 1; i++)
        {
            const vec2 &p2 = poly[i], &p3 = poly[i + 1];
            const vec2 e1 = p1 - p2, e2 = p3 - p2;

            const float w = e1.norm(),
                        w1 = std::abs(e1.dot(e2) / w),
                        w2 = std::abs(w - w1);

            const float h = std::abs(e2.cross(e1)) / w;
            const vec2 p4 = p2 + (p1 - p2) * w1 / w;

            const float i1 = w1 * h * (w1 * w1 / 3.f + h * h) / 4.f,
                        i2 = w2 * h * (w2 * w2 / 3.f + h * h) / 4.f;
            const float m1 = 0.5f * w1 * h, m2 = 0.5f * w2 * h;

            const vec2 cm1 = (p2 + p3 + p4) / 3.f, cm2 = (p1 + p3 + p4) / 3.f;

            const float icm1 = i1 + m1 * (cm1.sq_dist(poly.centroid()) - cm1.sq_dist(p3)),
                        icm2 = i2 + m2 * (cm2.sq_dist(poly.centroid()) - cm2.sq_dist(p3));
            if ((p1 - p3).cross(p4 - p3) < 0.f)
                inertia += icm1;
            else
                inertia -= icm1;
            if ((p4 - p3).cross(p2 - p3) < 0.f)
                inertia += icm2;
            else
                inertia -= icm2;
        }
        return std::abs(inertia) / poly.area();
    }

    void polygon2D::translate(const vec2 &dpos)
    {
        for (vec2 &v : m_vertices)
            v += dpos;
        m_centroid += dpos;
    }

    void polygon2D::pos(const vec2 &pos) { translate(pos - m_centroid); }

    const vec2 &polygon2D::support_vertex(const vec2 &direction) const
    {
        const vec2 &centroid = m_centroid;

        const auto cmp = [&direction, &centroid](const vec2 &v1, const vec2 &v2)
        { return direction.dot(v1 - centroid) < direction.dot(v2 - centroid); };

        const auto &support = std::max_element(m_vertices.begin(), m_vertices.end(), cmp);
        return *support;
    }

    bool polygon2D::is_convex() const
    {
        for (std::size_t i = 0; i < m_vertices.size(); i++)
        {
            const vec2 &prev = (*this)[i], &mid = (*this)[i + 1], &next = (*this)[i + 2];
            if ((mid - prev).cross(next - mid) < 0.f)
                return false;
        }
        return true;
    }

    bool polygon2D::contains_point(const vec2 &p) const
    {
        DBG_LOG_IF(!is_convex(), "Checking if a point is contained in a non convex polygon yields undefined behaviour.\n")
        for (std::size_t i = 0; i < m_vertices.size(); i++)
        {
            const vec2 &v1 = (*this)[i], &v2 = (*this)[i + 1];
            if (line_intersects_edge(v2, v1, p, m_centroid) && line_intersects_edge(p, m_centroid, v2, v1))
                return false;
        }
        return true;
    }

    bool polygon2D::contains_origin() const { return contains_point({0.f, 0.f}); }

    float polygon2D::distance_to(const vec2 &p) const { return towards_closest_edge_from(p).norm(); }

    float polygon2D::distance_to_origin() const { return distance_to({0.f, 0.f}); }

    vec2 polygon2D::towards_segment_from(const vec2 &p1, const vec2 &p2, const vec2 &p)
    {
        const float t = std::clamp((p - p1).dot(p2 - p1) / p1.sq_dist(p2), 0.f, 1.f);
        const vec2 proj = p1 + t * (p2 - p1);
        return proj - p;
    }

    vec2 polygon2D::towards_closest_edge_from(const vec2 &p) const
    {
        float min_dist = std::numeric_limits<float>::max();
        vec2 closest;
        for (std::size_t i = 0; i < m_vertices.size(); i++)
        {
            const vec2 towards = towards_segment_from((*this)[i], (*this)[i + 1], p);
            const float dist = towards.sq_norm();
            if (min_dist > dist)
            {
                min_dist = dist;
                closest = towards;
            }
        }
        return closest;
    }

    bool polygon2D::line_intersects_edge(const vec2 &l1, const vec2 &l2, const vec2 &v1, const vec2 &v2)
    {
        const float a = l2.y - l1.y, b = l1.x - l2.x;
        const float c = l2.x * l1.y - l1.x * l2.y;

        const float d1 = a * v1.x + b * v1.y + c;
        const float d2 = a * v2.x + b * v2.y + c;
        return !((d1 > 0.f && d2 > 0.f) || (d1 < 0.f && d2 < 0.f));
    }

    void polygon2D::sort_vertices_by_angle(const vec2 &centre_point)
    {
        const auto cmp = [&centre_point](const vec2 &v1, const vec2 &v2)
        { return (v1 - centre_point).angle() < (v2 - centre_point).angle(); };
        std::sort(m_vertices.begin(), m_vertices.end(), cmp);
    }

    void polygon2D::rotate(float dangle)
    {
        for (vec2 &v : m_vertices)
            v = (v - m_centroid).rotated(dangle) + m_centroid;
        m_angle += dangle;
        sort_vertices_by_angle(m_centroid);
    }
    void polygon2D::rotation(float angle) { rotate(angle - m_angle); }
    float polygon2D::rotation() const { return m_angle; }

    const std::vector<vec2> &polygon2D::vertices() const { return m_vertices; }

    std::size_t polygon2D::size() const { return m_vertices.size(); }

    const vec2 &polygon2D::centroid() const { return m_centroid; }

    float polygon2D::area() const { return m_area; }

    float polygon2D::inertia() const { return m_inertia; }

    const vec2 &polygon2D::operator[](const std::size_t index) const { return m_vertices[index % m_vertices.size()]; }

    std::vector<vec2> polygon2D::box(const float size)
    {
        return {
            {-size / 2.f, -size / 2.f},
            {size / 2.f, -size / 2.f},
            {size / 2.f, size / 2.f},
            {-size / 2.f, size / 2.f}};
    }
    std::vector<vec2> polygon2D::rect(const float width, const float height)
    {
        return {
            {-width / 2.f, -height / 2.f},
            {width / 2.f, -height / 2.f},
            {width / 2.f, height / 2.f},
            {-width / 2.f, height / 2.f}};
    }
    std::vector<vec2> polygon2D::circle(const float radius, const std::size_t partitions)
    {
        std::vector<vec2> vertices;
        vertices.reserve(partitions);

        const float dangle = 2.f * M_PI / partitions;
        for (std::size_t i = 0; i < partitions; i++)
        {
            const float angle = i * dangle;
            vertices.emplace_back(vec2(radius * std::sin(angle), radius * std::cos(angle)));
        }
        return vertices;
    }

    polygon2D polygon2D::minkowski_sum(const polygon2D &poly1, const polygon2D &poly2)
    {
        std::vector<vec2> sum;
        sum.reserve(poly1.size() + poly2.size());

        const auto cmp = [](const vec2 &v1, const vec2 &v2)
        { return v1.x < v2.x; };
        const std::size_t m1 = std::distance(poly1.vertices().begin(), std::min_element(poly1.vertices().begin(), poly1.vertices().end(), cmp));
        const std::size_t m2 = std::distance(poly2.vertices().begin(), std::min_element(poly2.vertices().begin(), poly2.vertices().end(), cmp));

        std::size_t i = 0, j = 0;
        while (i < poly1.size() || j < poly2.size())
        {
            const std::size_t index1 = i + m1, index2 = j + m2;
            sum.emplace_back(poly1[index1] + poly2[index2]);
            const float cross = (poly1[index1 + 1] - poly1[index1]).cross(poly2[index2 + 1] - poly2[index2]);
            if (cross >= 0.f)
                i++;
            if (cross <= 0.f)
                j++;
        }
        return polygon2D(sum);
    }

    polygon2D operator+(const polygon2D &poly) { return poly; }

    polygon2D &operator+(polygon2D &poly) { return poly; }

    polygon2D operator-(const polygon2D &poly)
    {
        std::vector<vec2> vertices;
        vertices.reserve(poly.size());
        for (const vec2 &v : poly.vertices())
            vertices.emplace_back(-v);
        return polygon2D(vertices);
    }

    polygon2D operator+(const polygon2D &poly1, const polygon2D &poly2) { return polygon2D::minkowski_sum(poly1, poly2); }

    polygon2D operator-(const polygon2D &poly1, const polygon2D &poly2) { return polygon2D::minkowski_sum(poly1, -poly2); }
}