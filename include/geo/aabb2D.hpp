#ifndef AABB2D_HPP
#define AABB2D_HPP

#include <glm/vec2.hpp>
#include <vector>
#ifdef YAML_CPP_COMPAT
#include <yaml-cpp/yaml.h>
#endif

namespace geo
{
class polygon;
class circle;
class aabb2D
{
  public:
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

#ifdef YAML_CPP_COMPAT
YAML::Emitter &operator<<(YAML::Emitter &out, const aabb2D &bb);
#endif
} // namespace geo

#ifdef YAML_CPP_COMPAT
namespace YAML
{
template <> struct convert<geo::aabb2D>
{
    static Node encode(const geo::aabb2D &bb);
    static bool decode(const Node &node, geo::aabb2D &bb);
};
} // namespace YAML
#endif

#endif