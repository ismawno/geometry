#ifndef GEO_CIRCLE_HPP
#define GEO_CIRCLE_HPP

#include "geo/shape2D.hpp"

namespace geo
{
class circle : public shape2D
{
  public:
    circle(float radius = 1.f);
    circle(const glm::vec2 &pos, float radius = 1.f);
    circle(const glm::vec2 &pos, float radius, float angle);

    glm::vec2 support_point(const glm::vec2 &direction) const override;
    bool contains_point(const glm::vec2 &p) const override;

    float area() const override;
    float inertia() const override;

    glm::vec2 closest_direction_from(const glm::vec2 &p) const override;

    float radius() const;
    void radius(float radius);

    bool is_convex() const override;

#ifdef KIT_USE_YAML_CPP
    YAML::Node encode() const override;
    bool decode(const YAML::Node &node) override;
#endif

  private:
    float m_radius;
    void update() override;
};
} // namespace geo

#endif