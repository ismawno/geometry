#ifndef POLYGON2D_HPP
#define POLYGON2D_HPP

#include "vec2.hpp"
#include <vector>
#include <array>
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

        const vec2 &support_vertex(const vec2 &direction) const;

        bool is_convex() const;
        bool contains_point(const vec2 &p) const;
        bool contains_origin() const;

        float distance_to(const vec2 &p) const;
        float distance_to_origin() const;

        vec2 towards_closest_edge_from(const vec2 &p) const;

        void rotate(float dangle);
        void rotation(float angle);
        float rotation() const;
        void sort_vertices();

        const std::vector<vec2> &vertices() const;

        std::size_t size() const;
        const vec2 &centroid() const;
        float area() const;
        float inertia() const;

        const vec2 &operator[](std::size_t index) const;

        static std::vector<vec2> box(float size);
        static std::vector<vec2> rect(float width, float height);
        static std::vector<vec2> circle(float radius, std::size_t partitions = 10);

        static polygon2D minkowski_sum(const polygon2D &poly1, const polygon2D &poly2);
        static vec2 centre_of_vertices(const polygon2D &poly);
        static vec2 centre_of_mass(const polygon2D &poly);
        static float area(const polygon2D &poly);
        static float inertia(const polygon2D &poly);

    private:
        std::vector<vec2> m_vertices;
        vec2 m_centroid;
        float m_angle = 0.f, m_area = 0.f, m_inertia = 0.f;

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