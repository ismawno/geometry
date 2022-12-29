#ifndef AABB2D_HPP
#define AABB2D_HPP

#include "vec2.hpp"
#include "polygon2D.hpp"
#include <vector>

namespace geo
{
    class aabb2D
    {
    public:
        aabb2D() = default;
        aabb2D(const alg::vec2 &min, const alg::vec2 &max);

        void bound(const std::vector<alg::vec2> &vertices);
        bool overlaps(const aabb2D &box) const;

        const alg::vec2 &min() const;
        const alg::vec2 &max() const;

    private:
        alg::vec2 m_min, m_max;
    };
}

#endif