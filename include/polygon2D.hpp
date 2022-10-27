#ifndef POLYGON2D_HPP
#define POLYGON2D_HPP

#include "shape2D.hpp"
#include <vector>

namespace geo
{
    class polygon2D : public shape2D
    {
    public:
        polygon2D() = delete;
        polygon2D(const std::vector<vec2> &vertices);
        polygon2D(const std::vector<vec2> &vertices, const vec2 &pos);

        void translate(const vec2 &dpos) override;
        static polygon2D minkowski_sum(const polygon2D &poly1, const polygon2D &poly2);
        const vec2 &support_vertex(const vec2 &direction) const override;

        bool is_convex() const override;
        bool contains_point(const vec2 &p) const override;
        bool contains_origin() const override;
        bool overlaps(const polygon2D &poly) const;

        float distance_to(const vec2 &p) const;
        float distance_to(const polygon2D &poly) const;
        float distance_to_origin() const;

        vec2 centre_of_mass() const;
        vec2 centre_of_vertices() const;

        const std::vector<vec2> &vertices() const;
        std::size_t size() const;

        const vec2 &operator[](std::size_t index) const;

    private:
        std::vector<vec2> m_vertices;

        void sort_vertices_by_angle();
        static float sq_dist_to_edge(const vec2 &p1, const vec2 &p2, const vec2 &p);
        float min_sq_dist_to_edge(const vec2 &p) const;
        static bool line_intersects_edge(const vec2 &l1, const vec2 &l2, const vec2 &v1, const vec2 &v2);
    };

    polygon2D operator+(const polygon2D &poly);

    polygon2D &operator+(polygon2D &poly);

    polygon2D operator-(const polygon2D &poly);

    polygon2D operator+(const polygon2D &poly1, const polygon2D &poly2);

    polygon2D operator-(const polygon2D &poly1, const polygon2D &poly2);
}

#endif