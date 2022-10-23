#ifndef BOX2D_HPP
#define BOX2D_HPP

#include "vec2.hpp"
#include <vector>

namespace geo
{
    using namespace vec;
    class box2D
    {
    public:
        box2D() = delete;
        box2D(const std::vector<vec2> &vertices);

        void bound();
        bool overlaps(const box2D &box) const;

        const vec2 &min() const;
        const vec2 &max() const;

    private:
        const std::vector<vec2> *m_vertices = nullptr;
        vec2 m_min, m_max;
    };
}

#endif