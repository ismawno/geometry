#pragma once

#include "geo/shape2D.hpp"
#include <vector>
#include <array>
#include <utility>

namespace geo
{
class polygon : public shape2D
{
  public:
    polygon(const std::vector<glm::vec2> &vertices = square(1.f));
    polygon(const kit::transform2D &transform, const std::vector<glm::vec2> &vertices = square(1.f));

    glm::vec2 support_point(const glm::vec2 &direction) const override;

    bool is_convex() const override;
    bool contains_point(const glm::vec2 &p) const override;

    glm::vec2 closest_direction_from(const glm::vec2 &p) const override;

    const glm::vec2 &locals(std::size_t index) const;
    const glm::vec2 &globals(std::size_t index) const;

    const std::vector<glm::vec2> &locals() const;
    const std::vector<glm::vec2> &globals() const;

    std::size_t size() const;
    float area() const override;
    float inertia() const override;

    static std::vector<glm::vec2> square(float size);
    static std::vector<glm::vec2> rect(float width, float height);
    static std::vector<glm::vec2> ngon(float radius, std::uint32_t sides);

    static polygon minkowski_sum(const polygon &poly1, const polygon &poly2);
    static polygon minkowski_difference(const polygon &poly1, const polygon &poly2);

#ifdef KIT_USE_YAML_CPP
    YAML::Node encode() const override;
    bool decode(const YAML::Node &node) override;
#endif

  private:
    std::vector<glm::vec2> m_local_vertices, m_global_vertices;
    float m_area = 0.f, m_inertia = 0.f;

    void on_shape_transform_update(const glm::mat3 &transform) override;

    void sort_global_vertices();
    glm::vec2 initialize_properties_and_local_vertices();
};

polygon operator-(const polygon &poly);
polygon operator+(const polygon &poly1, const polygon &poly2);
polygon operator-(const polygon &poly1, const polygon &poly2);
} // namespace geo
