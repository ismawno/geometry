#pragma once

#include "kit/container/dynarray.hpp"
#include "kit/utility/type_constraints.hpp"
#include <glm/vec2.hpp>

namespace geo
{
template <std::size_t N> class polygon;
template <std::size_t N>
    requires(N >= 3)
class vertices2D
{
  public:
    template <class... VectorArgs>
        requires kit::NoCopyCtorOverride<vertices2D, VectorArgs...>
    vertices2D(VectorArgs &&...args) : m_vertices(std::forward<VectorArgs>(args)...)
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

    operator const kit::dynarray<glm::vec2, N> &() const
    {
        return m_vertices;
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
} // namespace geo