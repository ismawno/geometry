#include "geo/pch.hpp"
#include "geo/aabb2D.hpp"
#include "geo/polygon.hpp"
#include "geo/circle.hpp"

namespace geo
{
aabb2D::aabb2D(const polygon &poly)
{
    bound(poly);
}
aabb2D::aabb2D(const circle &circ)
{
    bound(circ);
}

aabb2D::aabb2D(const glm::vec2 &point) : aabb2D(point, point)
{
}
aabb2D::aabb2D(const glm::vec2 &min, const glm::vec2 &max) : min(min), max(max)
{
    KIT_ASSERT_WARN(min.x <= max.x && min.y <= max.y,
                    "Vector min: {0}, {1} is greater than vector max {0}, {1} in aabb", min.x, min.y, max.x, max.y)
}

void aabb2D::bound(const polygon &poly)
{
    min = glm::vec2(FLT_MAX);
    max = -glm::vec2(FLT_MAX);
    for (const glm::vec2 &v : poly.globals())
    {
        if (min.x > v.x)
            min.x = v.x;
        if (min.y > v.y)
            min.y = v.y;
        if (max.x < v.x)
            max.x = v.x;
        if (max.y < v.y)
            max.y = v.y;
    }
}
void aabb2D::bound(const circle &circ)
{
    min = circ.centroid() - glm::vec2(circ.radius);
    max = circ.centroid() + glm::vec2(circ.radius);
}

glm::vec2 aabb2D::dimension() const
{
    return max - min;
}

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
    return aabb2D({std::min(bb1.min.x, bb2.min.x), std::min(bb1.min.y, bb2.min.y)},
                  {std::max(bb1.max.x, bb2.max.x), std::max(bb1.max.y, bb2.max.y)});
}
aabb2D operator-(const aabb2D &bb1, const aabb2D &bb2)
{
    return aabb2D({std::max(bb1.min.x, bb2.min.x), std::max(bb1.min.y, bb2.min.y)},
                  {std::min(bb1.max.x, bb2.max.x), std::min(bb1.max.y, bb2.max.y)});
}

#ifdef KIT_USE_YAML_CPP
YAML::Node aabb2D::serializer::encode(const aabb2D &aabb) const
{
    YAML::Node node;
    node["Min"] = aabb.min;
    node["Max"] = aabb.max;
    node["Min"].SetStyle(YAML::EmitterStyle::Flow);
    node["Max"].SetStyle(YAML::EmitterStyle::Flow);
    return node;
}
bool aabb2D::serializer::decode(const YAML::Node &node, aabb2D &aabb) const
{
    if (!node.IsMap() || node.size() != 2)
        return false;

    aabb = {node["Min"].as<glm::vec2>(), node["Max"].as<glm::vec2>()};
    return true;
}
#endif
} // namespace geo