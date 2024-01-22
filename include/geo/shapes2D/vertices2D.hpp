#pragma once

#include "kit/container/dynarray.hpp"
#include "kit/utility/type_constraints.hpp"
#include <glm/vec2.hpp>

namespace geo
{
template <std::size_t N>
    requires(N >= 3)
class vertex_container2D
{
  public:
    template <class... VectorArgs>
        requires kit::NoCopyCtorOverride<vertex_container2D, VectorArgs...>
    vertex_container2D(VectorArgs &&...args) : m_vertices(std::forward<VectorArgs>(args)...)
    {
    }

    const glm::vec2 &operator[](const std::size_t index) const
    {
        return m_vertices[index % m_vertices.size()];
    }

    auto begin() const
    {
        return m_vertices.begin();
    }
    auto end() const
    {
        return m_vertices.end();
    }

    std::size_t size() const
    {
        return m_vertices.size();
    }

  private:
    glm::vec2 &operator[](const std::size_t index)
    {
        return m_vertices[index % m_vertices.size()];
    }

    auto begin()
    {
        return m_vertices.begin();
    }
    auto end()
    {
        return m_vertices.end();
    }

    kit::dynarray<glm::vec2, N> m_vertices;

    friend class polygon<N>;
};

template <std::size_t Capacity>
    requires(Capacity >= 3)
class vertices2D
{
  public:
    template <class... VectorArgs>
        requires kit::NoCopyCtorOverride<vertices2D, VectorArgs...>
    vertices2D(VectorArgs &&...args)
        : globals(std::forward<VectorArgs>(args)...), locals(globals.size()), edges(globals.size()),
          normals(globals.size())
    {
    }

    std::size_t size() const
    {
        return globals.size();
    }

    vertex_container2D<Capacity> globals;
    vertex_container2D<Capacity> locals;
    vertex_container2D<Capacity> edges;
    vertex_container2D<Capacity> normals;
};
} // namespace geo