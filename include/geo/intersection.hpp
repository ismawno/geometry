#ifndef INTERSECTION_HPP
#define INTERSECTION_HPP

#include "geo/circle.hpp"
#include "geo/aabb2D.hpp"
#include <glm/vec2.hpp>
#include <vector>
#include <utility>

namespace geo
{
    bool gjk(const shape2D &sh1, const shape2D &sh2, std::vector<glm::vec2> &simplex);
    glm::vec2 epa(const shape2D &sh1, const shape2D &sh2, std::vector<glm::vec2> &simplex);
    std::pair<glm::vec2, glm::vec2> contact_points(const shape2D &sh1, const shape2D &sh2, const glm::vec2 &mtv);

    bool may_intersect(const shape2D &sh1, const shape2D &sh2);
    bool intersect(const aabb2D &bb1, const aabb2D &bb2);
    bool intersect(const circle &c1, const circle &c2);
    glm::vec2 mtv(const circle &c1, const circle &c2);
    std::pair<glm::vec2, glm::vec2> contact_points(const circle &c1, const circle &c2);
}

#endif