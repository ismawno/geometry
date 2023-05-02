#include "geo/pch.hpp"
#include "geo/shape2D.hpp"

namespace geo
{
    shape2D::shape2D(const glm::vec2 &centroid) : m_centroid(centroid) {}

    void shape2D::translate(const glm::vec2 &dpos) { m_centroid += dpos; }
    void shape2D::pos(const glm::vec2 &pos) { translate(pos - m_centroid); }

    const glm::vec2 &shape2D::centroid() const { return m_centroid; }

    void shape2D::rotate(const float dangle) { m_angle += dangle; }
    void shape2D::rotation(const float angle) { rotate(angle - m_angle); }

    YAML::Emitter &operator<<(YAML::Emitter &out, const shape2D &sh)
    {
        sh.write(out);
        return out;
    }
    float shape2D::rotation() const { return m_angle; }

#ifdef HAS_YAML_CPP
    void shape2D::write(YAML::Emitter &out) const
    {
        out << YAML::Key << "centroid" << YAML::Value << m_centroid;
        out << YAML::Key << "angle" << YAML::Value << m_angle;
    }
#endif
}