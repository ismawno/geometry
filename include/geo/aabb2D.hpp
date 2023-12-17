#ifndef GEO_AABB2D_HPP
#define GEO_AABB2D_HPP

#include <glm/vec2.hpp>
#include <vector>
#include "kit/interface/serialization.hpp"

namespace geo
{
class polygon;
class circle;
struct aabb2D
{
#ifdef KIT_USE_YAML_CPP
    class serializer : public kit::serializer<aabb2D>
    {
      public:
        YAML::Node encode(const aabb2D &aabb) const override;
        bool decode(const YAML::Node &node, aabb2D &aabb) const override;
    };
#endif
    aabb2D(const polygon &poly);
    aabb2D(const circle &c);

    aabb2D(const glm::vec2 &point = glm::vec2(0.f));
    aabb2D(const glm::vec2 &min, const glm::vec2 &max);

    void bound(const polygon &poly);
    void bound(const circle &c);

    glm::vec2 dimension() const;

    glm::vec2 min;
    glm::vec2 max;

    aabb2D &operator+=(const aabb2D &bb);
    aabb2D &operator-=(const aabb2D &bb);
};

aabb2D operator+(const aabb2D &bb1, const aabb2D &bb2);
aabb2D operator-(const aabb2D &bb1, const aabb2D &bb2);

} // namespace geo

#endif