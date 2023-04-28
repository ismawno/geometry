#ifndef AABB2D_HPP
#define AABB2D_HPP

#include <glm/vec2.hpp>
#include <vector>

namespace geo
{
    class polygon;
    class circle;
    class aabb2D
    {
    public:
        aabb2D(const std::vector<glm::vec2> &vertices);
        aabb2D(const polygon &poly);
        aabb2D(const circle &c);

        aabb2D(const glm::vec2 &point);
        aabb2D(const glm::vec2 &min, const glm::vec2 &max);

        void bound(const std::vector<glm::vec2> &vertices);
        void bound(const polygon &poly);
        void bound(const circle &c);

        const glm::vec2 &min() const;
        const glm::vec2 &max() const;

    private:
        glm::vec2 m_min{0.f}, m_max{0.f};
    };
}

#endif