#include "geo/shape2D.hpp"

namespace geo
{
    void shape2D::translate(const glm::vec2 &dpos) { m_centroid += dpos; }
    void shape2D::pos(const glm::vec2 &pos) { translate(pos - m_centroid); }

    const glm::vec2 &shape2D::centroid() const { return m_centroid; }

    void shape2D::rotate(const float dangle) { m_angle += dangle; }
    void shape2D::rotation(const float angle) { rotate(angle - m_angle); }
    float shape2D::rotation() const { return m_angle; }
}