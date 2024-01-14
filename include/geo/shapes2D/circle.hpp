#pragma once

#include "geo/shapes2D/shape2D.hpp"

namespace geo
{
class circle final : public shape2D
{
  public:
    circle(float radius = 1.f);
    circle(const kit::transform2D<float> &transform, float radius = 1.f);

    float radius;

    glm::vec2 support_point(const glm::vec2 &direction) const override;
    bool contains_point(const glm::vec2 &p) const override;

    float area() const override;
    float inertia() const override;
    void bound() override;

    glm::vec2 closest_direction_from(const glm::vec2 &p) const override;

    bool is_convex() const override;

#ifdef KIT_USE_YAML_CPP
    YAML::Node encode() const override;
    bool decode(const YAML::Node &node) override;
#endif
};
} // namespace geo
