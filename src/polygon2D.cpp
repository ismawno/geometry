#include "polygon2D.hpp"
#include "debug.h"
#include <algorithm>
#include <cstdint>
#include <cmath>
#include <limits>

namespace geo
{
    polygon2D::polygon2D(const vec2 &pos,
                         const std::vector<vec2> &vertices) : polygon2D(vertices)
    {
        DBG_EXIT_IF(m_vertices.size() < 3, "Cannot make polygon with less than 3 vertices.\n")
        const vec2 cvert = centre_of_vertices();
        translate(-cvert);
        sort_vertices_by_angle();
        m_centroid = centre_of_mass();
        translate(pos - m_centroid);
    }
    polygon2D::polygon2D(const std::vector<vec2> &vertices) : m_vertices(vertices)
    {
        DBG_EXIT_IF(m_vertices.size() < 3, "Cannot make polygon with less than 3 vertices.\n")
        const vec2 cvert = centre_of_vertices();
        translate(-cvert);
        sort_vertices_by_angle();
        m_centroid = centre_of_mass();
        translate(cvert);
    }

    vec2 polygon2D::centre_of_mass() const
    {
        const vec2 &v0 = m_vertices[0];

        vec2 num;
        float den = 0.f;
        for (std::size_t i = 1; i < m_vertices.size() - 1; i++)
        {
            const vec2 edg1 = m_vertices[i] - v0, edg2 = m_vertices[i + 1] - v0;
            const float cross = std::abs(edg1.cross(edg2));
            num += (edg1 + edg2) * cross;
            den += cross;
        }
        return v0 + num / (3.f * den);
    }

    vec2 polygon2D::centre_of_vertices() const
    {
        vec2 centre;
        for (const vec2 &v : m_vertices)
            centre += v;
        return centre / m_vertices.size();
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

    bool polygon2D::overlaps(const polygon2D &poly) const { return gjk(*this, poly); }

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

    std::pair<vec2, vec2> polygon2D::touch_points(const polygon2D &poly1, const polygon2D &poly2)
    {
        const vec2 mtv = (poly2 - poly1).towards_closest_edge_from({0.f, 0.f});
        vec2 t1, t2;
        float min_dist = std::numeric_limits<float>::max();
        for (const vec2 &v : poly1.vertices())
        {
            const vec2 towards = poly2.towards_closest_edge_from(v + mtv);
            const float dist = towards.sq_norm();
            if (min_dist > dist)
            {
                min_dist = dist;
                t1 = v;
                t2 = v + mtv;
            }
        }
        for (const vec2 &v : poly2.vertices())
        {
            const vec2 towards = poly1.towards_closest_edge_from(v - mtv);
            const float dist = towards.sq_norm();
            if (min_dist > dist)
            {
                min_dist = dist;
                t2 = v;
                t1 = v - mtv;
            }
        }
        return {t1, t2};
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
    std::vector<vec2> &polygon2D::vertices() { return m_vertices; }

    std::size_t polygon2D::size() const { return m_vertices.size(); }

    const vec2 &polygon2D::centroid() const { return m_centroid; }

    const vec2 &polygon2D::operator[](const std::size_t index) const { return m_vertices[index % m_vertices.size()]; }

    bool polygon2D::gjk(const polygon2D &poly1, const polygon2D &poly2)
    {
        vec2 dir = poly2.centroid() - poly1.centroid();
        std::vector<vec2> simplex;
        simplex.reserve(3);

        const vec2 supp = poly1.support_vertex(dir) - poly2.support_vertex(-dir);
        dir = -supp;
        simplex.emplace_back(supp);

        for (;;)
        {
            const vec2 A = poly1.support_vertex(dir) - poly2.support_vertex(-dir);
            if (A.dot(dir) < 0.f)
                return false;
            simplex.emplace_back(A);
            if (simplex.size() == 2)
                line_case(simplex, dir);
            else if (triangle_case(simplex, dir))
                return true;
        }
    }
    void polygon2D::line_case(const std::vector<vec2> &simplex, vec2 &dir)
    {
        const vec2 AB = simplex[0] - simplex[1], AO = -simplex[1];
        dir = vec2::triple_cross(AB, AO, AB);
    }
    bool polygon2D::triangle_case(std::vector<vec2> &simplex, vec2 &dir)
    {
        const vec2 AB = simplex[1] - simplex[2], AC = simplex[0] - simplex[2], AO = -simplex[2];
        const vec2 ABperp = vec2::triple_cross(AC, AB, AB);
        if (ABperp.dot(AO) > 0.f)
        {
            simplex.erase(simplex.begin());
            return false;
        }
        const vec2 ACperp = vec2::triple_cross(AB, AC, AC);
        if (ACperp.dot(AO) > 0.f)
        {
            simplex.erase(simplex.begin() + 1);
            dir = ACperp;
            return false;
        }
        return true;
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