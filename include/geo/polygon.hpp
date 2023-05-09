#ifndef POLYGON_HPP
#define POLYGON_HPP

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
        polygon(const glm::vec2 &centroid,
                const std::vector<glm::vec2> &vertices = box(1.f));
        polygon(const glm::vec2 &centroid, float angle,
                const std::vector<glm::vec2> &vertices = box(1.f));

        glm::vec2 support_point(const glm::vec2 &direction) const override;

        bool is_convex() const;
        bool contains_point(const glm::vec2 &p) const override;

        aabb2D bounding_box() const override;
        glm::vec2 closest_direction_from(const glm::vec2 &p) const override;

        void sort_vertices();

        const std::vector<glm::vec2> &locals() const;
        const glm::vec2 &local(std::size_t index) const;
        glm::vec2 local(const glm::vec2 &p) const;

        std::vector<glm::vec2> globals() const;
        glm::vec2 global(std::size_t index) const;
        glm::vec2 global(const glm::vec2 &p) const;

        glm::vec2 translated(std::size_t index) const;
        glm::vec2 translated(const glm::vec2 &p) const;

        glm::vec2 rotated(std::size_t index) const;
        glm::vec2 rotated(const glm::vec2 &p) const;

        std::size_t size() const;
        float area() const override;
        float inertia() const override;

        glm::vec2 operator[](std::size_t index) const;

#ifdef HAS_YAML_CPP

#endif

        static std::vector<glm::vec2> box(float size);
        static std::vector<glm::vec2> rect(float width, float height);
        static std::vector<glm::vec2> ngon(float radius, std::uint32_t sides);

        static polygon minkowski_sum(const polygon &poly1, const polygon &poly2);
        static polygon minkowski_difference(const polygon &poly1, const polygon &poly2);

    private:
        std::vector<glm::vec2> m_local_vertices;
        float m_area = 0.f, m_inertia = 0.f;

#ifdef HAS_YAML_CPP
        void write(YAML::Emitter &out) const override;
        YAML::Node encode() const override;
        bool decode(const YAML::Node &node) override;
        friend struct YAML::convert<polygon>;
#endif
    };

    polygon operator-(const polygon &poly);
    polygon operator+(const polygon &poly1, const polygon &poly2);
    polygon operator-(const polygon &poly1, const polygon &poly2);
}

#ifdef HAS_YAML_CPP
namespace YAML
{
    template <>
    struct convert<geo::polygon>
    {
        static Node encode(const geo::polygon &poly);
        static bool decode(const Node &node, geo::polygon &poly);
    };
}
#endif

#endif