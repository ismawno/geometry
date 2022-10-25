#ifndef POLYGON2D_HPP
#define POLYGON2D_HPP

#include "vec2.hpp"
#include <vector>

namespace geo
{
    using namespace vec;
    class polygon2D
    {
    public:
        polygon2D() = delete;
        polygon2D(const std::vector<vec2> &vertices);
        polygon2D(const std::vector<vec2> &vertices, const vec2 &pos);

        void translate(const vec2 &dpos);
        static polygon2D minkowski_sum(const polygon2D &poly1, const polygon2D &poly2);
        bool is_convex() const;
        bool contains_point(const vec2 &p) const;
        bool contains_origin() const;

        const std::vector<vec2> &vertices() const;
        const vec2 &geometrical_centre() const;
        std::size_t size() const;

        const vec2 &operator[](std::size_t index) const;

    private:
        std::vector<vec2> m_vertices;
        vec2 m_centre;

        const vec2 &compute_centre();
        void sort_vertices_by_angle();
    };

    polygon2D operator+(const polygon2D &poly);

    polygon2D &operator+(polygon2D &poly);

    polygon2D operator-(const polygon2D &poly);

    polygon2D operator+(const polygon2D &poly1, const polygon2D &poly2);

    polygon2D operator-(const polygon2D &poly1, const polygon2D &poly2);
}

#endif