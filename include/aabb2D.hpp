#ifndef AABB2D_HPP
#define AABB2D_HPP

#include "glm/vec2.hpp"
#include <vector>

namespace geo
{
    class aabb2D
    {
    public:
        aabb2D() = default;
        aabb2D(const glm::vec2 &point);
        aabb2D(const glm::vec2 &min, const glm::vec2 &max);

        void bound(const std::vector<glm::vec2> &vertices);
        bool overlaps(const aabb2D &box) const;

        const glm::vec2 &min() const;
        const glm::vec2 &max() const;

    private:
        glm::vec2 m_min{0.f}, m_max{0.f};
    };
}

#endif