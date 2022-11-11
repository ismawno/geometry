#ifndef BOX2D_HPP
#define BOX2D_HPP

#include "vec2.hpp"
#include "polygon2D.hpp"
#include <vector>

namespace geo
{
    using namespace vec;
    class box2D
    {
    public:
        box2D() = default;

        void bound(const std::vector<vec2> &vertices, const vec2 &centroid);
        bool overlaps(const box2D &box) const;
        void recentre(const vec2 &centroid);

        const vec2 &min() const;
        const vec2 &max() const;

    private:
        vec2 m_min, m_max, m_centroid;
    };
}

#endif