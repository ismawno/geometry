#pragma once

#include "kit/container/dynarray.hpp"
#include "kit/utility/type_constraints.hpp"
#include <glm/vec2.hpp>

namespace geo
{
template <std::size_t N> class polygon;

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

template <> class vertex_container2D<SIZE_MAX>
{
  public:
    vertex_container2D(std::vector<glm::vec2> *vertices, const std::size_t start)
        : m_vertices(vertices), m_start(start), m_size(vertices->size())
    {
    }

    const glm::vec2 &operator[](const std::size_t index) const
    {
        return m_vertices->at(m_start + index % m_size);
    }

    auto begin() const
    {
        return m_vertices->begin() + m_start;
    }
    auto end() const
    {
        return m_vertices->begin() + m_start + m_size;
    }

    std::size_t size() const
    {
        return m_size;
    }

  private:
    glm::vec2 &operator[](const std::size_t index)
    {
        return m_vertices->at(m_start + index % m_size);
    }

    auto begin()
    {
        return m_vertices->begin() + m_start;
    }
    auto end()
    {
        return m_vertices->begin() + m_start + m_size;
    }

    std::vector<glm::vec2> *m_vertices;
    std::size_t m_start, m_size;

    friend class polygon<SIZE_MAX>;
};

template <std::size_t Capacity = SIZE_MAX>
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

template <> class vertices2D<SIZE_MAX>
{
  public:
    template <class... VectorArgs>
        requires kit::NoCopyCtorOverride<vertices2D, VectorArgs...>
    vertices2D(VectorArgs &&...args)
        : m_vertices(std::forward<VectorArgs>(args)...), globals(&m_vertices, 0),
          locals(&m_vertices, m_vertices.size()), edges(&m_vertices, 2 * m_vertices.size()),
          normals(&m_vertices, 3 * m_vertices.size())
    {
        m_vertices.resize(4 * m_vertices.size());
    }
    vertices2D(const vertices2D &other)
        : m_vertices(other.m_vertices), globals(&m_vertices, 0), locals(&m_vertices, m_vertices.size()),
          edges(&m_vertices, 2 * m_vertices.size()), normals(&m_vertices, 3 * m_vertices.size())
    {
        m_vertices.resize(4 * m_vertices.size());
    }
    vertices2D(vertices2D &&other)
        : m_vertices(std::move(other.m_vertices)), globals(&m_vertices, 0), locals(&m_vertices, m_vertices.size()),
          edges(&m_vertices, 2 * m_vertices.size()), normals(&m_vertices, 3 * m_vertices.size())
    {
        m_vertices.resize(4 * m_vertices.size());
    }

    vertices2D &operator=(const vertices2D &other)
    {
        m_vertices = other.m_vertices;
        return *this;
    }
    vertices2D &operator=(vertices2D &&other)
    {
        m_vertices = std::move(other.m_vertices);
        return *this;
    }

    std::size_t size() const
    {
        return globals.size();
    }

  private:
    std::vector<glm::vec2> m_vertices;

  public:
    vertex_container2D<SIZE_MAX> globals;
    vertex_container2D<SIZE_MAX> locals;
    vertex_container2D<SIZE_MAX> edges;
    vertex_container2D<SIZE_MAX> normals;
};
} // namespace geo