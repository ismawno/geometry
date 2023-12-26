#pragma once

#include "geo/shapes2D/circle.hpp"
#include "geo/shapes2D/aabb2D.hpp"
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
mtv_result epa(const shape2D &sh1, const shape2D &sh2, const std::array<glm::vec2, 3> &simplex,
               float threshold = 1.e-3f);

glm::vec2 mtv_support_contact_point(const shape2D &sh1, const shape2D &sh2, const glm::vec2 &mtv);
bool may_intersect(const shape2D &sh1, const shape2D &sh2);

bool intersects(const aabb2D &bb1, const aabb2D &bb2);
bool intersects(const circle &c1, const circle &c2);

mtv_result mtv(const circle &c1, const circle &c2);
glm::vec2 radius_distance_contact_point(const circle &c1, const circle &c2);
} // namespace geo
