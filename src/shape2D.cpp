#include "shape2D.hpp"

namespace geo
{
    shape2D::shape2D(const vec2 &pos) : m_centre(pos) {}

    const vec2 &shape2D::centre() const { return m_centre; }
}