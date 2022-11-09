#ifndef POLYGON2D_HPP
#define POLYGON2D_HPP

#include "vec2.hpp"
#include <vector>
#include <utility>

namespace geo
{
    using namespace vec;
    class polygon2D
    {
    public:
        polygon2D(const std::vector<vec2> &vertices = {{-1.f, -1.f}, {1.f, -1.f}, {1.f, 1.f}, {-1.f, 1.f}});
        polygon2D(const vec2 &pos,
                  const std::vector<vec2> &vertices = {{-1.f, -1.f}, {1.f, -1.f}, {1.f, 1.f}, {-1.f, 1.f}});

        void translate(const vec2 &dpos);
        void pos(const vec2 &pos);

        static polygon2D minkowski_sum(const polygon2D &poly1, const polygon2D &poly2);
        const vec2 &support_vertex(const vec2 &direction) const;

        bool is_convex() const;
        bool contains_point(const vec2 &p) const;
        bool contains_origin() const;
        bool overlaps(const polygon2D &poly) const;

        float distance_to(const vec2 &p) const;
        float distance_to(const polygon2D &poly) const;
        float distance_to_origin() const;

        vec2 towards_closest_edge_from(const vec2 &p) const;
        std::pair<vec2, vec2> separation_points(const polygon2D &poly) const;
        vec2 centre_of_mass() const;
        vec2 centre_of_vertices() const;

        void rotate(float dangle);
        void rotation(float angle);
        float rotation() const;

        const std::vector<vec2> &vertices() const;
        std::vector<vec2> &vertices();

        std::size_t size() const;
        const vec2 &centroid() const;

        const vec2 &operator[](std::size_t index) const;

    private:
        std::vector<vec2> m_vertices;
        vec2 m_centroid;
        float m_angle = 0.f;

        void sort_vertices_by_angle();
        static vec2 towards_segment_from(const vec2 &p1, const vec2 &p2, const vec2 &p);
        static bool line_intersects_edge(const vec2 &l1, const vec2 &l2, const vec2 &v1, const vec2 &v2);
    };

    polygon2D operator+(const polygon2D &poly);

    polygon2D &operator+(polygon2D &poly);

    polygon2D operator-(const polygon2D &poly);

    polygon2D operator+(const polygon2D &poly1, const polygon2D &poly2);

    polygon2D operator-(const polygon2D &poly1, const polygon2D &poly2);
}

#endif