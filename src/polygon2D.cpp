#include "polygon2D.hpp"
#include <algorithm>
#include <cstdint>

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
        return m_centre;
    }

    void polygon2D::translate(const vec2 &dpos)
    {
        for (vec2 &v : m_vertices)
            v += dpos;
        m_centre += dpos;
    }

    polygon2D polygon2D::minkowski_sum(const polygon2D &poly) const
    {
        const std::vector<vec2> &vertices = poly.vertices();
        std::vector<vec2> sum;
        sum.reserve(m_vertices.size() + vertices.size());

        std::size_t i = 0, j = 0;
        while (i < m_vertices.size() || j < m_vertices.size())
        {
            sum.emplace_back(m_vertices[i] + vertices[j]);
            const float cross = (m_vertices[i + 1] - m_vertices[i]).cross(vertices[j + 1] - vertices[j]);
            if (cross >= 0.f)
                i++;
            if (cross <= 0.f)
                j++;
        }
        return polygon2D(sum);
    }

    static bool cmp(const vec2 &v1, const vec2 &v2) { return v1.angle() < v2.angle(); }

    void polygon2D::sort_vertices_by_angle() { std::sort(m_vertices.begin(), m_vertices.end(), cmp); }

    const std::vector<vec2> &polygon2D::vertices() const { return m_vertices; }

    const vec2 &polygon2D::geometrical_centre() const { return m_centre; }

    const vec2 &polygon2D::operator[](const std::size_t index) const { return m_vertices[index % m_vertices.size()]; }
}