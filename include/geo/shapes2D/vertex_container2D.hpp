#pragma once

#include "kit/debug/log.hpp"
#include "kit/utility/type_constraints.hpp"
#include <glm/vec2.hpp>
#include <array>

namespace geo
{
template <std::size_t N>
    requires(N >= 3)
class vertex_container2D
{
  public:
    template <std::size_t M>
        requires(M <= N)
    vertex_container2D(const std::array<glm::vec2, M> &vertices) : m_size(M)
    {
        for (std::size_t i = 0; i < M; ++i)
            m_vertices[i] = vertices[i];
    }
    template <kit::Iterator<glm::vec2> It> vertex_container2D(It begin, It end) : m_size(0)
    {
        for (auto it = begin; it != end; ++it)
            m_vertices[m_size++] = *it;
        KIT_ASSERT_ERROR(m_size >= 3, "Polygon must have at least 3 vertices")
    }

    const glm::vec2 &operator[](const std::size_t index) const
    {
        KIT_ASSERT_ERROR(index < m_vertices.size(), "Index exceeds container size: {0}", index)
        return m_vertices[index];
    }
    glm::vec2 &operator[](const std::size_t index) const
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

    void resize(std::size_t size)
    {
        m_size = size;
    }
    std::size_t size() const
    {
        return m_size;
    }

  private:
    std::array<glm::vec2, N> m_vertices;
    std::size_t m_size;
};

template <typename Container>
concept V2Vector = std::is_same_v<Container, std::vector<glm::vec2>>;

template <> class vertex_container2D<SIZE_MAX>
{
  public:
    template <V2Vector Container>
    vertex_container2D(Container &&vertices) : m_vertices(std::forward<Container>(vertices))
    {
    }

    template <kit::Iterator<glm::vec2> It> vertex_container2D(It begin, It end) : m_vertices(begin, end)
    {
    }

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