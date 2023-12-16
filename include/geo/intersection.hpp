#ifndef GEO_INTERSECTION_HPP
#define GEO_INTERSECTION_HPP

#include "geo/circle.hpp"
#include "geo/aabb2D.hpp"
#include <glm/vec2.hpp>
#include <array>
#include <utility>

namespace geo
{
struct gjk_result
{
    bool intersect;
    std::array<glm::vec2, 3> simplex;
};
struct mtv_result
{
    bool valid;
    glm::vec2 mtv;
};

gjk_result gjk(const shape2D &sh1, const shape2D &sh2);
mtv_result epa(const shape2D &sh1, const shape2D &sh2, const std::array<glm::vec2, 3> &simplex);

glm::vec2 contact_point(const shape2D &sh1, const shape2D &sh2, const glm::vec2 &mtv);
bool may_intersect(const shape2D &sh1, const shape2D &sh2);

bool intersects(const aabb2D &bb1, const aabb2D &bb2);
bool intersects(const circle &c1, const circle &c2);

mtv_result mtv(const circle &c1, const circle &c2);
glm::vec2 contact_point(const circle &c1, const circle &c2);
} // namespace geo

#endif