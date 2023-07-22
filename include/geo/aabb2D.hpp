#ifndef GEO_AABB2D_HPP
#define GEO_AABB2D_HPP

#include <glm/vec2.hpp>
#include <vector>
#include "kit/interface/serialization.hpp"

namespace geo
{
class polygon;
class circle;
class aabb2D
{
  public:
#ifdef KIT_USE_YAML_CPP
    class serializer : public kit::serializer<aabb2D>
    {
      public:
        YAML::Node encode(const aabb2D &aabb) const override;
        bool decode(const YAML::Node &node, aabb2D &aabb) const override;
    };
#endif
    aabb2D() = default;
    aabb2D(const polygon &poly);
    aabb2D(const circle &c);

    aabb2D(const glm::vec2 &point);
    aabb2D(const glm::vec2 &min, const glm::vec2 &max);

    void bound(const polygon &poly);
    void bound(const circle &c);

    const glm::vec2 &min() const;
    const glm::vec2 &max() const;

    aabb2D &operator+=(const aabb2D &bb);
    aabb2D &operator-=(const aabb2D &bb);

  private:
    glm::vec2 m_min{0.f}, m_max{0.f};
};

aabb2D operator+(const aabb2D &bb1, const aabb2D &bb2);
aabb2D operator-(const aabb2D &bb1, const aabb2D &bb2);

} // namespace geo

#endif