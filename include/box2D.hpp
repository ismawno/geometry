#ifndef BOX2D_HPP
#define BOX2D_HPP

#include "vec2.hpp"
#include "polygon2D.hpp"
#include <vector>

namespace geo
{
    class box2D
    {
    public:
        box2D() = default;

        void bound(const std::vector<alg::vec2> &vertices, const alg::vec2 &centroid);
        bool overlaps(const box2D &box) const;
        void recentre(const alg::vec2 &centroid);

        const alg::vec2 &min() const;
        const alg::vec2 &max() const;

    private:
        alg::vec2 m_min, m_max, m_centroid;
    };
}

#endif