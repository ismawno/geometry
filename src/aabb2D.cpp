#include "geo/pch.hpp"
#include "geo/aabb2D.hpp"
#include "geo/polygon.hpp"
#include "geo/circle.hpp"
#include "debug/debug.hpp"

namespace geo
{
    aabb2D::aabb2D(const std::vector<glm::vec2> &vertices) { bound(vertices); }
    aabb2D::aabb2D(const polygon &poly) { bound(poly); }
    aabb2D::aabb2D(const circle &c) { bound(c); }

    aabb2D::aabb2D(const glm::vec2 &point) : aabb2D(point, point) {}
    aabb2D::aabb2D(const glm::vec2 &min, const glm::vec2 &max) : m_min(min), m_max(max)
    {
        DBG_ASSERT(min.x <= max.x && min.y <= max.y, "min vector must be lower than max vector!")
    }

    void aabb2D::bound(const std::vector<glm::vec2> &vertices)
    {
        m_min = glm::vec2(std::numeric_limits<float>::max());
        m_max = -glm::vec2(std::numeric_limits<float>::max());
        for (const glm::vec2 &v : vertices)
        {
            if (m_min.x > v.x)
                m_min.x = v.x;
            if (m_min.y > v.y)
                m_min.y = v.y;
            if (m_max.x < v.x)
                m_max.x = v.x;
            if (m_max.y < v.y)
                m_max.y = v.y;
        }
    }
    void aabb2D::bound(const polygon &poly) { bound(poly.vertices()); }
    void aabb2D::bound(const circle &c)
    {
        m_min = c.centroid() - glm::vec2(c.radius());
        m_max = c.centroid() + glm::vec2(c.radius());
    }

    const glm::vec2 &aabb2D::min() const { return m_min; }
    const glm::vec2 &aabb2D::max() const { return m_max; }

    aabb2D &aabb2D::operator+=(const aabb2D &bb)
    {
        *this = *this + bb;
        return *this;
    }
    aabb2D &aabb2D::operator-=(const aabb2D &bb)
    {
        *this = *this - bb;
        return *this;
    }

    aabb2D operator+(const aabb2D &bb1, const aabb2D &bb2)
    {
        return aabb2D({std::min(bb1.min().x, bb2.min().x),
                       std::min(bb1.min().y, bb2.min().y)},
                      {std::max(bb1.max().x, bb2.max().x),
                       std::max(bb1.max().y, bb2.max().y)});
    }
    aabb2D operator-(const aabb2D &bb1, const aabb2D &bb2)
    {
        return aabb2D({std::max(bb1.min().x, bb2.min().x),
                       std::max(bb1.min().y, bb2.min().y)},
                      {std::min(bb1.max().x, bb2.max().x),
                       std::min(bb1.max().y, bb2.max().y)});
    }

#ifdef HAS_YAML_CPP
    YAML::Emitter &operator<<(YAML::Emitter &out, const aabb2D &bb)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "min" << YAML::Value << bb.min();
        out << YAML::Key << "max" << YAML::Value << bb.max();
        out << YAML::EndMap;
        return out;
    }
#endif
}

#ifdef HAS_YAML_CPP
namespace YAML
{
    Node convert<geo::aabb2D>::encode(const geo::aabb2D &bb)
    {
        Node node;
        node.push_back(bb.min());
        node.push_back(bb.max());
        return node;
    }
    bool convert<geo::aabb2D>::decode(const Node &node, geo::aabb2D &bb)
    {
        if (!node.IsSequence() || node.size() != 2)
            return false;

        bb = {node[0].as<glm::vec2>(), node[1].as<glm::vec2>()};
        return true;
    };
}
#endif