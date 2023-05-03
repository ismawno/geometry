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
    YAML::Node shape2D::encode() const
    {
        YAML::Node node;
        node["centroid"] = m_centroid;
        node["centroid"].SetStyle(YAML::EmitterStyle::Flow);
        node["angle"] = m_angle;
        return node;
    }
    bool shape2D::decode(const YAML::Node &node)
    {
        if (!node.IsMap() || node.size() < 2)
            return false;

        m_centroid = node["centroid"].as<glm::vec2>();
        m_angle = node["angle"].as<float>();
        return true;
    }
#endif
}

#ifdef HAS_YAML_CPP
namespace YAML
{
    Node convert<geo::shape2D>::encode(const geo::shape2D &sh)
    {
        return sh.encode();
    }
    bool convert<geo::shape2D>::decode(const Node &node, geo::shape2D &sh)
    {
        return sh.decode(node);
    };
}
#endif