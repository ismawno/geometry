#pragma once

#include "kit/debug/log.hpp"
#include "kit/utility/type_constraints.hpp"
#include <glm/vec2.hpp>
#include <array>

namespace geo
{
template <std::size_t Capacity = SIZE_MAX>
    requires(Capacity >= 3)
class vertices2D
{
  public:
    vertices2D(const std::size_t size = 0) : m_size(size)
    {
    }

    template <std::size_t OtherCapacity>
        requires(OtherCapacity <= Capacity && OtherCapacity >= 3)
    vertices2D(const std::array<glm::vec2, OtherCapacity> &vertices) : m_size(OtherCapacity)
    {
        for (std::size_t i = 0; i < OtherCapacity; ++i)
            m_vertices[i] = vertices[i];
    }
    template <kit::Iterator<glm::vec2> It> vertices2D(It begin, It end) : m_size(0)
    {
        for (auto it = begin; it != end; ++it)
        {
            KIT_ASSERT_ERROR(m_size < Capacity, "Vertex capacity exceeded: {0}", Capacity)
            m_vertices[m_size++] = *it;
        }
        KIT_ASSERT_ERROR(m_size >= 3, "Polygon must have at least 3 vertices")
    }
    vertices2D(std::initializer_list<glm::vec2> vertices) : vertices2D(vertices.begin(), vertices.end())
    {
    }

    template <std::size_t OtherCapacity> vertices2D(const vertices2D<OtherCapacity> &other) : m_size(other.size())
    {
        KIT_ASSERT_ERROR(other.size() <= Capacity, "Vertex capacity exceeded: {0}", Capacity)
        for (std::size_t i = 0; i < m_size; ++i)
            m_vertices[i] = other[i];
    }

    template <std::size_t OtherCapacity> vertices2D &operator=(const vertices2D<OtherCapacity> &other)
    {
        KIT_ASSERT_ERROR(other.size() <= Capacity, "Vertex capacity exceeded: {0}", Capacity)
        m_size = other.size();
        for (std::size_t i = 0; i < m_size; ++i)
            m_vertices[i] = other[i];
        return *this;
    }

    void push_back(const glm::vec2 &vertex)
    {
        KIT_ASSERT_ERROR(m_size < Capacity, "Vertex capacity exceeded: {0}", Capacity)
        m_vertices[m_size++] = vertex;
    }

    const glm::vec2 &operator[](const std::size_t index) const
    {
        KIT_ASSERT_ERROR(index < m_vertices.size(), "Index exceeds container size: {0}", index)
        return m_vertices[index];
    }
    glm::vec2 &operator[](const std::size_t index)
    {
        KIT_ASSERT_ERROR(index < m_vertices.size(), "Index exceeds container size: {0}", index)
        return m_vertices[index];
    }

    auto begin() const
    {
        return m_vertices.begin();
    }
    auto end() const
    {
        return m_vertices.begin() + m_size;
    }

    auto begin()
    {
        return m_vertices.begin();
    }
    auto end()
    {
        return m_vertices.begin() + m_size;
    }

    void resize(std::size_t size)
    {
        m_size = size;
    }
    std::size_t size() const
    {
        return m_size;
    }

  private:
    std::array<glm::vec2, Capacity> m_vertices;
    std::size_t m_size;
};

template <typename Container>
concept V2Vector = std::is_same_v<Container, std::vector<glm::vec2>>;

template <> class vertices2D<SIZE_MAX>
{
  public:
    vertices2D(const std::size_t size = 0);

    template <V2Vector Container> vertices2D(Container &&vertices) : m_vertices(std::forward<Container>(vertices))
    {
    }

    template <std::size_t Capacity>
    vertices2D(const std::array<glm::vec2, Capacity> &other) : m_vertices(other.begin(), other.end())
    {
    }
    template <kit::Iterator<glm::vec2> It> vertices2D(It begin, It end) : m_vertices(begin, end)
    {
    }
    vertices2D(std::initializer_list<glm::vec2> vertices);

    template <std::size_t OtherCapacity> vertices2D(const vertices2D<OtherCapacity> &other)
    {
        m_vertices.resize(other.size());
        for (std::size_t i = 0; i < m_vertices.size(); ++i)
            m_vertices[i] = other[i];
    }

    template <std::size_t OtherCapacity> vertices2D &operator=(const vertices2D<OtherCapacity> &other)
    {
        m_vertices.resize(other.size());
        for (std::size_t i = 0; i < m_vertices.size(); ++i)
            m_vertices[i] = other[i];
        return *this;
    }

    void push_back(const glm::vec2 &vertex);

    const glm::vec2 &operator[](const std::size_t index) const;
    glm::vec2 &operator[](const std::size_t index);

    auto begin() const
    {
        return m_vertices.begin();
    }
    auto end() const
    {
        return m_vertices.end();
    }

    auto begin()
    {
        return m_vertices.begin();
    }
    auto end()
    {
        return m_vertices.end();
    }

    void resize(std::size_t size);
    std::size_t size() const;

  private:
    std::vector<glm::vec2> m_vertices;
};
} // namespace geo