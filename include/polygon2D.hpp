#ifndef POLYGON2D_HPP
#define POLYGON2D_HPP

#include "vec2.hpp"
#include "saveable.hpp"
#include <vector>
#include <array>
#include <utility>

namespace geo
{
    class polygon2D : public ini::saveable
    {
    public:
        polygon2D(const std::vector<alg::vec2> &vertices = box(1.f));
        polygon2D(const alg::vec2 &pos,
                  const std::vector<alg::vec2> &vertices = box(1.f));

        void translate(const alg::vec2 &dpos);
        void pos(const alg::vec2 &pos);

        const alg::vec2 &support_vertex(const alg::vec2 &direction) const;

        bool is_convex() const;
        bool contains_point(const alg::vec2 &p) const;
        bool contains_origin() const;

        float distance_to(const alg::vec2 &p) const;
        float distance_to_origin() const;

        alg::vec2 towards_closest_edge_from(const alg::vec2 &p) const;

        void rotate(float dangle);
        void rotation(float angle);
        float rotation() const;
        void sort_vertices();

        void write(ini::output &out) const override;
        void read(ini::input &in) override;

        const std::vector<alg::vec2> &vertices() const;

        std::size_t size() const;
        const alg::vec2 &centroid() const;
        float area() const;
        float inertia() const;

        alg::vec2 relative(std::size_t index) const;
        const alg::vec2 &operator[](std::size_t index) const;

        static std::vector<alg::vec2> box(float size);
        static std::vector<alg::vec2> rect(float width, float height);
        static std::vector<alg::vec2> ngon(float radius, std::uint32_t sides);

        static polygon2D minkowski_sum(const polygon2D &poly1, const polygon2D &poly2);
        static alg::vec2 centre_of_vertices(const polygon2D &poly);
        static alg::vec2 centre_of_mass(const polygon2D &poly);
        static float area(const polygon2D &poly);
        static float inertia(const polygon2D &poly);

    private:
        std::vector<alg::vec2> m_vertices;
        alg::vec2 m_centroid;
        float m_angle = 0.f, m_area = 0.f, m_inertia = 0.f;

        static alg::vec2 towards_segment_from(const alg::vec2 &p1,
                                              const alg::vec2 &p2,
                                              const alg::vec2 &p);
        static bool line_intersects_edge(const alg::vec2 &l1,
                                         const alg::vec2 &l2,
                                         const alg::vec2 &v1,
                                         const alg::vec2 &v2);
    };

    polygon2D operator+(const polygon2D &poly);

    polygon2D &operator+(polygon2D &poly);

    polygon2D operator-(const polygon2D &poly);

    polygon2D operator+(const polygon2D &poly1, const polygon2D &poly2);

    polygon2D operator-(const polygon2D &poly1, const polygon2D &poly2);
}

#endif