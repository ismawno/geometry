#include "shape2D.hpp"

namespace geo
{
    shape2D::shape2D(const vec2 &pos) : m_centroid(pos) {}

    const vec2 &shape2D::centroid() const { return m_centroid; }
}