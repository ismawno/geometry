#include "polygon2D.hpp"
#include <algorithm>
#include <cstdint>
#include <cmath>

namespace geo
{
    polygon2D::polygon2D(const std::vector<vec2> &vertices,
                         const vec2 &pos) : polygon2D(vertices)
    {
        compute_centre();
        translate(-m_centre);
        sort_vertices_by_angle();
        translate(pos);
    }
    polygon2D::polygon2D(const std::vector<vec2> &vertices) : m_vertices(vertices)
    {
        const vec2 prev_centre = compute_centre();
        translate(-m_centre);
        sort_vertices_by_angle();
        translate(prev_centre);
    }

    const vec2 &polygon2D::polygon2D::compute_centre()
    {
        m_centre = {0.f, 0.f};
        for (const vec2 &v : m_vertices)
            m_centre += v;
        m_centre /= m_vertices.size();
        return m_centre;
    }

    void polygon2D::translate(const vec2 &dpos)
    {
        for (vec2 &v : m_vertices)
            v += dpos;
        m_centre += dpos;
    }

    polygon2D polygon2D::minkowski_sum(const polygon2D &poly1, const polygon2D &poly2)
    {
        std::vector<vec2> sum;
        sum.reserve(poly1.size() + poly2.size());

        std::size_t i = 0, j = 0;
        while (i < poly1.size() || j < poly2.size())
        {
            sum.emplace_back(poly1[i] + poly2[j]);
            const float cross = (poly1[i + 1] - poly1[i]).cross(poly2[j + 1] - poly2[j]);
            if (cross >= 0.f)
                i++;
            if (cross <= 0.f)
                j++;
        }
        return polygon2D(sum);
    }

    bool polygon2D::is_convex() const
    {
        for (std::size_t i = 0; i < m_vertices.size() - 2; i++)
        {
            const vec2 &prev = m_vertices[i], &mid = m_vertices[i + 1], &next = m_vertices[i + 2];
            const vec2 accel = (next - mid) - (mid - prev);
            if (accel.dot(m_centre - mid) < 0.f)
                return false;
        }
        return true;
    }

    bool polygon2D::contains_point(const vec2 &p) const
    {
        const auto cmp = [&p](const vec2 &v1, const vec2 &v2)
        { return (v1 - p).angle() <= (v2 - p).angle(); };
        const auto &[min, max] = std::minmax_element(m_vertices.begin(), m_vertices.end(), cmp);
        return max->angle() - min->angle() >= M_PI;
    }

    bool polygon2D::contains_origin() const { return contains_point({0.f, 0.f}); }

    void polygon2D::sort_vertices_by_angle()
    {
        const auto cmp = [](const vec2 &v1, const vec2 &v2)
        { return v1.angle() <= v2.angle(); };
        std::sort(m_vertices.begin(), m_vertices.end(), cmp);
    }

    const std::vector<vec2> &polygon2D::vertices() const { return m_vertices; }

    const vec2 &polygon2D::geometrical_centre() const { return m_centre; }

    std::size_t polygon2D::size() const { return m_vertices.size(); }

    const vec2 &polygon2D::operator[](const std::size_t index) const { return m_vertices[index % m_vertices.size()]; }

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