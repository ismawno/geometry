#include "polygon.hpp"
#include "debug.hpp"
#include <algorithm>
#include <cstdint>
#include <cmath>
#include <limits>

namespace geo
{
    polygon::polygon(const std::vector<alg::vec2> &vertices) : m_vertices(vertices)
    {
        DBG_ASSERT(m_vertices.size() >= 3, "Cannot make polygon with less than 3 vertices - vertices: %zu\n", m_vertices.size())
        m_centroid = centre_of_mass(*this);
        m_area = area(*this);
        m_inertia = inertia(*this);
        sort_vertices();
    }

    polygon::polygon(const alg::vec2 &pos,
                     const std::vector<alg::vec2> &vertices) : polygon(vertices)
    {
        this->pos(pos);
    }

    alg::vec2 polygon::centre_of_vertices(const polygon &poly)
    {
        alg::vec2 centre;
        for (const alg::vec2 &v : poly.vertices())
            centre += v;
        return centre / poly.size();
    }

    alg::vec2 polygon::centre_of_mass(const polygon &poly)
    {
        const alg::vec2 &p1 = poly[0];
        alg::vec2 num, den;
        for (std::size_t i = 1; i < poly.size() - 1; i++)
        {
            const alg::vec2 e1 = poly[i] - p1, e2 = poly[i + 1] - p1;
            const float cross = std::abs(e1.cross(e2));
            num += (e1 + e2) * cross;
            den += cross;
        }
        return p1 + num / (3.f * den);
    }

    float polygon::area(const polygon &poly)
    {
        float area = 0.f;
        const alg::vec2 &p1 = poly[0];
        for (std::size_t i = 1; i < poly.size() - 1; i++)
        {
            const alg::vec2 e1 = poly[i] - p1, e2 = poly[i + 1] - p1;
            area += std::abs(e1.cross(e2));
        }
        return area / 2.f;
    }

    float polygon::inertia(const polygon &poly)
    {
        const alg::vec2 &p1 = poly[0];
        float inertia = 0.f;
        for (std::size_t i = 1; i < poly.size() - 1; i++)
        {
            const alg::vec2 &p2 = poly[i], &p3 = poly[i + 1];
            const alg::vec2 e1 = p1 - p2, e2 = p3 - p2;

            const float w = e1.norm(),
                        w1 = std::abs(e1.dot(e2) / w),
                        w2 = std::abs(w - w1);

            const float h = std::abs(e2.cross(e1)) / w;
            const alg::vec2 p4 = p2 + (p1 - p2) * w1 / w;

            const float i1 = w1 * h * (w1 * w1 / 3.f + h * h) / 4.f,
                        i2 = w2 * h * (w2 * w2 / 3.f + h * h) / 4.f;
            const float m1 = 0.5f * w1 * h, m2 = 0.5f * w2 * h;

            const alg::vec2 cm1 = (p2 + p3 + p4) / 3.f, cm2 = (p1 + p3 + p4) / 3.f;

            const float icm1 = i1 + m1 * (cm1.sq_dist(poly.centroid()) - cm1.sq_dist(p3)),
                        icm2 = i2 + m2 * (cm2.sq_dist(poly.centroid()) - cm2.sq_dist(p3));
            if ((p1 - p3).cross(p4 - p3) < 0.f)
                inertia += icm1;
            else
                inertia -= icm1;
            if ((p4 - p3).cross(p2 - p3) < 0.f)
                inertia += icm2;
            else
                inertia -= icm2;
        }
        return std::abs(inertia) / poly.area();
    }

    void polygon::translate(const alg::vec2 &dpos)
    {
        for (alg::vec2 &v : m_vertices)
            v += dpos;
        m_centroid += dpos;
    }

    void polygon::pos(const alg::vec2 &pos) { translate(pos - m_centroid); }

    const alg::vec2 &polygon::support_vertex(const alg::vec2 &direction) const
    {
        const alg::vec2 &centroid = m_centroid;

        const auto cmp = [&direction, &centroid](const alg::vec2 &v1, const alg::vec2 &v2)
        { return direction.dot(v1 - centroid) < direction.dot(v2 - centroid); };

        const auto &support = std::max_element(m_vertices.begin(), m_vertices.end(), cmp);
        return *support;
    }

    bool polygon::is_convex() const
    {
        for (std::size_t i = 0; i < m_vertices.size(); i++)
        {
            const alg::vec2 &prev = (*this)[i], &mid = (*this)[i + 1], &next = (*this)[i + 2];
            if ((mid - prev).cross(next - mid) < 0.f)
                return false;
        }
        return true;
    }

    bool polygon::contains_point(const alg::vec2 &p) const
    {
        DBG_LOG_IF(!is_convex(), "Checking if a point is contained in a non convex polygon yields undefined behaviour.\n")
        for (std::size_t i = 0; i < m_vertices.size(); i++)
        {
            const alg::vec2 &v1 = (*this)[i], &v2 = (*this)[i + 1];
            if (line_intersects_edge(v2, v1, p, m_centroid) && line_intersects_edge(p, m_centroid, v2, v1))
                return false;
        }
        return true;
    }

    bool polygon::contains_origin() const { return contains_point(alg::vec2::zero); }

    float polygon::distance_to(const alg::vec2 &p) const
    {
        return towards_closest_edge_from(p).norm();
    }

    float polygon::distance_to_origin() const { return distance_to(alg::vec2::zero); }

    alg::vec2 polygon::towards_segment_from(const alg::vec2 &p1,
                                            const alg::vec2 &p2,
                                            const alg::vec2 &p)
    {
        const float t = std::clamp((p - p1).dot(p2 - p1) / p1.sq_dist(p2), 0.f, 1.f);
        const alg::vec2 proj = p1 + t * (p2 - p1);
        return proj - p;
    }

    alg::vec2 polygon::towards_closest_edge_from(const alg::vec2 &p) const
    {
        float min_dist = std::numeric_limits<float>::max();
        alg::vec2 closest;
        for (std::size_t i = 0; i < m_vertices.size(); i++)
        {
            const alg::vec2 towards = towards_segment_from((*this)[i], (*this)[i + 1], p);
            const float dist = towards.sq_norm();
            if (min_dist > dist)
            {
                min_dist = dist;
                closest = towards;
            }
        }
        return closest;
    }

    bool polygon::line_intersects_edge(const alg::vec2 &l1,
                                       const alg::vec2 &l2,
                                       const alg::vec2 &v1,
                                       const alg::vec2 &v2)
    {
        const float a = l2.y - l1.y, b = l1.x - l2.x;
        const float c = l2.x * l1.y - l1.x * l2.y;

        const float d1 = a * v1.x + b * v1.y + c;
        const float d2 = a * v2.x + b * v2.y + c;
        return !((d1 > 0.f && d2 > 0.f) || (d1 < 0.f && d2 < 0.f));
    }

    void polygon::sort_vertices()
    {
        const alg::vec2 &centroid = m_centroid;
        const auto cmp = [&centroid](const alg::vec2 &v1, const alg::vec2 &v2)
        { return (v1 - centroid).angle() < (v2 - centroid).angle(); };
        std::sort(m_vertices.begin(), m_vertices.end(), cmp);
    }

    void polygon::write(ini::output &out) const
    {
        out.write("angle", m_angle);
        std::size_t index = 0;
        const std::string key = "vertex";

        for (const alg::vec2 &v : m_vertices)
        {
            out.write(key + std::to_string(index) + "x", v.x);
            out.write(key + std::to_string(index++) + "y", v.y);
        }
    }
    void polygon::read(ini::input &in)
    {
        m_angle = in.readf("angle");
        std::vector<alg::vec2> vertices;
        vertices.reserve(m_vertices.capacity());

        std::size_t index = 0;
        const std::string key = "vertex";
        while (true)
        {
            const std::string kx = key + std::to_string(index) + "x",
                              ky = key + std::to_string(index++) + "y";
            DBG_ASSERT((in.contains_key(kx) && in.contains_key(ky)) ||
                           (!in.contains_key(kx) && !in.contains_key(ky)),
                       "Vector key only contains a component of the vector! Weird.\n")
            if (!in.contains_key(kx) || !in.contains_key(ky)) // Just for ick reasons
                break;
            vertices.emplace_back(in.readf(kx), in.readf(ky));
        }
        *this = geo::polygon(vertices);
    }

    void polygon::rotate(float dangle)
    {
        for (alg::vec2 &v : m_vertices)
            v = (v - m_centroid).rotated(dangle) + m_centroid;
        m_angle += dangle;
    }
    void polygon::rotation(float angle) { rotate(angle - m_angle); }
    float polygon::rotation() const { return m_angle; }

    const std::vector<alg::vec2> &polygon::vertices() const { return m_vertices; }

    std::size_t polygon::size() const { return m_vertices.size(); }

    const alg::vec2 &polygon::centroid() const { return m_centroid; }

    float polygon::area() const { return m_area; }

    float polygon::inertia() const { return m_inertia; }

    alg::vec2 polygon::relative(const std::size_t index) const { return this->operator[](index) - m_centroid; }

    const alg::vec2 &polygon::operator[](const std::size_t index) const
    {
        return m_vertices[index % m_vertices.size()];
    }

    std::vector<alg::vec2> polygon::box(const float size)
    {
        return {
            {-size / 2.f, -size / 2.f},
            {size / 2.f, -size / 2.f},
            {size / 2.f, size / 2.f},
            {-size / 2.f, size / 2.f}};
    }
    std::vector<alg::vec2> polygon::rect(const float width, const float height)
    {
        return {
            {-width / 2.f, -height / 2.f},
            {width / 2.f, -height / 2.f},
            {width / 2.f, height / 2.f},
            {-width / 2.f, height / 2.f}};
    }
    std::vector<alg::vec2> polygon::ngon(const float radius, const std::uint32_t sides)
    {
        std::vector<alg::vec2> vertices;
        vertices.reserve(sides);

        const float dangle = 2.f * M_PI / sides;
        for (std::size_t i = 0; i < sides; i++)
        {
            const float angle = i * dangle;
            vertices.emplace_back(alg::vec2(radius * std::sinf(angle), radius * std::cosf(angle)));
        }
        return vertices;
    }

    polygon polygon::minkowski_sum(const polygon &poly1, const polygon &poly2)
    {
        std::vector<alg::vec2> sum;
        sum.reserve(poly1.size() + poly2.size());

        const auto cmp = [](const alg::vec2 &v1, const alg::vec2 &v2)
        { return v1.x < v2.x; };
        const std::size_t m1 = std::distance(poly1.vertices().begin(),
                                             std::min_element(poly1.vertices().begin(),
                                                              poly1.vertices().end(), cmp)),

                          m2 = std::distance(poly2.vertices().begin(),
                                             std::min_element(poly2.vertices().begin(),
                                                              poly2.vertices().end(), cmp));

        std::size_t i = 0, j = 0;
        while (i < poly1.size() || j < poly2.size())
        {
            const std::size_t index1 = i + m1, index2 = j + m2;
            sum.emplace_back(poly1[index1] + poly2[index2]);
            const float cross = (poly1[index1 + 1] - poly1[index1]).cross(poly2[index2 + 1] - poly2[index2]);
            if (cross >= 0.f)
                i++;
            if (cross <= 0.f)
                j++;
        }
        return polygon(sum);
    }

    polygon operator+(const polygon &poly) { return poly; }

    polygon &operator+(polygon &poly) { return poly; }

    polygon operator-(const polygon &poly)
    {
        std::vector<alg::vec2> vertices;
        vertices.reserve(poly.size());
        for (const alg::vec2 &v : poly.vertices())
            vertices.emplace_back(-v);
        return polygon(vertices);
    }

    polygon operator+(const polygon &poly1, const polygon &poly2)
    {
        return polygon::minkowski_sum(poly1, poly2);
    }

    polygon operator-(const polygon &poly1, const polygon &poly2)
    {
        return polygon::minkowski_sum(poly1, -poly2);
    }
}