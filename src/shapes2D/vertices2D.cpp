#include "geo/internal/pch.hpp"
#include "geo/shapes2D/vertices2D.hpp"

namespace geo
{
vertices2D<>::vertices2D(const std::size_t size) : m_vertices(size)
{
}
vertices2D<>::vertices2D(const std::initializer_list<glm::vec2> vertices) : m_vertices(vertices)
{
}

void vertices2D<>::push_back(const glm::vec2 &vertex)
{
    m_vertices.push_back(vertex);
}

const glm::vec2 &vertices2D<SIZE_MAX>::operator[](const std::size_t index) const
{
    return m_vertices[index];
}

glm::vec2 &vertices2D<SIZE_MAX>::operator[](const std::size_t index)
{
    return m_vertices[index];
}

std::size_t vertices2D<SIZE_MAX>::size() const
{
    return m_vertices.size();
}
void vertices2D<SIZE_MAX>::resize(std::size_t size)
{
    m_vertices.resize(size);
}
} // namespace geo