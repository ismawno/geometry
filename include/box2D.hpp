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
        box2D(const polygon2D &poly);

        void bound();
        bool overlaps(const box2D &box) const;
        void recentre();

        const vec2 &min() const;
        const vec2 &max() const;

    private:
        const polygon2D &m_poly;
        vec2 m_min, m_max, m_centroid;
    };
}

#endif