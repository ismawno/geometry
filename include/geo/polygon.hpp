#ifndef POLYGON_HPP
#define POLYGON_HPP

#include "glm/vec2.hpp"
#include "ini/saveable.hpp"
#include "geo/shape2D.hpp"
#include <vector>
#include <array>
#include <utility>

namespace geo
{
    class polygon : public shape2D
    {
    public:
        polygon(const std::vector<glm::vec2> &vertices = box(1.f));
        polygon(const glm::vec2 &pos,
                const std::vector<glm::vec2> &vertices = box(1.f));

        void translate(const glm::vec2 &dpos) override;

        glm::vec2 support_point(const glm::vec2 &direction) const override;

        bool is_convex() const;
        bool contains_point(const glm::vec2 &p) const override;
        bool contains_origin() const override;

        glm::vec2 closest_direction_from(const glm::vec2 &p) const override;

        void rotate(float dangle) override;
        void sort_vertices();

        void write(ini::output &out) const override;
        void read(ini::input &in) override;

        const std::vector<glm::vec2> &vertices() const;

        std::size_t size() const;
        float area() const override;
        float inertia() const override;

        glm::vec2 relative(std::size_t index) const;
        const glm::vec2 &operator[](std::size_t index) const;

        static std::vector<glm::vec2> box(float size);
        static std::vector<glm::vec2> rect(float width, float height);
        static std::vector<glm::vec2> ngon(float radius, std::uint32_t sides);

        static polygon minkowski_sum(const polygon &poly1, const polygon &poly2);
        static glm::vec2 centre_of_vertices(const polygon &poly);
        static glm::vec2 centre_of_mass(const polygon &poly);
        static float area(const polygon &poly);
        static float inertia(const polygon &poly);

    private:
        std::vector<glm::vec2> m_vertices;
        float m_area = 0.f, m_inertia = 0.f;

        static glm::vec2 towards_segment_from(const glm::vec2 &p1,
                                              const glm::vec2 &p2,
                                              const glm::vec2 &p);
        static bool line_intersects_edge(const glm::vec2 &l1,
                                         const glm::vec2 &l2,
                                         const glm::vec2 &v1,
                                         const glm::vec2 &v2);
    };

    polygon operator-(const polygon &poly);

    polygon operator+(const polygon &poly1, const polygon &poly2);

    polygon operator-(const polygon &poly1, const polygon &poly2);
}

#endif