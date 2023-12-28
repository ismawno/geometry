#pragma once
#ifdef KIT_USE_YAML_CPP

#include "geo/shapes2D/polygon.hpp"
#include "geo/shapes2D/circle.hpp"
#include "kit/serialization/yaml/codec.hpp"
#include "kit/serialization/yaml/glm.hpp"
#include "kit/serialization/yaml/transform.hpp"

template <> struct kit::yaml::codec<geo::aabb2D>
{
    static YAML::Node encode(const geo::aabb2D &aabb)
    {
        YAML::Node node;
        node["Min"] = aabb.min;
        node["Max"] = aabb.max;
        node["Min"].SetStyle(YAML::EmitterStyle::Flow);
        node["Max"].SetStyle(YAML::EmitterStyle::Flow);
        return node;
    }
    static bool decode(const YAML::Node &node, geo::aabb2D &aabb)
    {
        if (!node.IsMap() || node.size() != 2)
            return false;

        aabb = {node["Min"].as<glm::vec2>(), node["Max"].as<glm::vec2>()};
        return true;
    }
};

template <> struct kit::yaml::codec<geo::circle>
{
    static YAML::Node encode(const geo::circle &circ)
    {
        YAML::Node node;
        node["Transform"] = circ.transform();
        node["Radius"] = circ.radius;

        return node;
    }
    static bool decode(const YAML::Node &node, geo::circle &circ)
    {
        if (!node.IsMap() || node.size() != 2)
            return false;
        circ = {node["Transform"].as<kit::transform2D<float>>(), node["Radius"].as<float>()};
        return true;
    }
};

template <> struct kit::yaml::codec<geo::polygon>
{
    static YAML::Node encode(const geo::polygon &poly)
    {
        YAML::Node node;
        node["Transform"] = poly.transform();
        for (std::size_t i = 0; i < poly.size(); i++)
        {
            node["Vertices"].push_back(poly.local(i));
            node["Vertices"][i].SetStyle(YAML::EmitterStyle::Flow);
        }
        return node;
    }
    static bool decode(const YAML::Node &node, geo::polygon &poly)
    {
        if (!node.IsMap() || node.size() != 2)
            return false;
        YAML::Node node_v = node["Vertices"];

        std::vector<glm::vec2> vertices;
        vertices.reserve(node_v.size());
        for (std::size_t i = 0; i < node_v.size(); i++)
            vertices.push_back(node_v[i].as<glm::vec2>());

        const kit::transform2D<float> transform = node["Transform"].as<kit::transform2D<float>>();
        poly = {transform, vertices};
        return true;
    }
};
#endif