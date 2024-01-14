#include "geo/internal/pch.hpp"
#include "geo/shapes2D/vertices2D.hpp"

namespace geo
{
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