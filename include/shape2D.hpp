#ifndef SHAPE2D_HPP
#define SHAPE2D_HPP

#include "vec2.hpp"

namespace geo
{
    using namespace vec;
    class shape2D
    {
    public:
        shape2D(const vec2 &pos = {0.f, 0.f});

        virtual void translate(const vec2 &dpos) = 0;
        virtual const vec2 &support_vertex(const vec2 &direction) const = 0;

        virtual bool is_convex() const = 0;
        virtual bool contains_point(const vec2 &p) const = 0;
        virtual bool contains_origin() const = 0;

        const vec2 &centre() const;

    protected:
        vec2 m_centre;
    };
}

#endif