#include "geo/pch.hpp"
#include "geo/intersection.hpp"
#include "geo/polygon.hpp"

#define EPA_EPSILON 1.e-3f

namespace geo
{
    static float cross(const glm::vec2 &v1, const glm::vec2 &v2) { return v1.x * v2.y - v1.y * v2.x; }
    static glm::vec2 triple_cross(const glm::vec2 &v1, const glm::vec2 &v2, const glm::vec2 &v3)
    {
        const float crs = cross(v1, v2);
        return glm::vec2(-v3.y * crs, v3.x * crs);
    }

    struct arr3
    {
        std::array<glm::vec2, 3> data;
        std::size_t size = 0;

        void push(const glm::vec2 &val)
        {
            data[size++] = val;
            DBG_ASSERT_ERROR(size <= 3, "Array size exceeds 3!")
        }
        void erase(const std::size_t index)
        {
            DBG_ASSERT_ERROR(size > 0, "Cannot erase element of empty array!")
            for (std::size_t i = index; i < size - 1; i++)
                data[i] = data[i + 1];
            --size;
        }
    };

    static void line_case(const arr3 &simplex, glm::vec2 &dir)
    {
        const glm::vec2 AB = simplex.data[0] - simplex.data[1], AO = -simplex.data[1];
        dir = triple_cross(AB, AO, AB);
    }

    static bool triangle_case(arr3 &simplex, glm::vec2 &dir)
    {
        const glm::vec2 AB = simplex.data[1] - simplex.data[2], AC = simplex.data[0] - simplex.data[2], AO = -simplex.data[2];
        const glm::vec2 ABperp = triple_cross(AC, AB, AB);
        if (glm::dot(ABperp, AO) >= 0.f)
        {
            simplex.erase(0);
            dir = ABperp;
            return false;
        }
        const glm::vec2 ACperp = triple_cross(AB, AC, AC);
        if (glm::dot(ACperp, AO) >= 0.f)
        {
            simplex.erase(1);
            dir = ACperp;
            return false;
        }
        return true;
    }

    std::optional<std::array<glm::vec2, 3>> gjk(const shape2D &sh1, const shape2D &sh2)
    {
        PERF_FUNCTION()
        DBG_ASSERT_WARN(!dynamic_cast<const circle *>(&sh1) || !dynamic_cast<const circle *>(&sh2), "Using gjk algorithm to check if two circles are intersecting, which is overkill")

        arr3 simplex;
        glm::vec2 dir = sh2.centroid() - sh1.centroid();
        const glm::vec2 supp = sh1.support_point(dir) - sh2.support_point(-dir);
        simplex.push(supp);
        dir = -supp;

        for (;;)
        {
            const glm::vec2 A = sh1.support_point(dir) - sh2.support_point(-dir);
            if (glm::dot(A, dir) <= 0.f)
                return {};
            simplex.push(A);
            if (simplex.size == 2)
                line_case(simplex, dir);
            else if (triangle_case(simplex, dir))
                return simplex.data;
        }
    }

    std::optional<glm::vec2> epa(const shape2D &sh1, const shape2D &sh2, const std::array<glm::vec2, 3> &simplex)
    {
        PERF_FUNCTION()

        std::vector<glm::vec2> hull;
        hull.reserve(10);
        hull.insert(hull.end(), simplex.begin(), simplex.end());

        float min_dist = FLT_MAX;
        glm::vec2 mtv{0.f, 0.f};
        for (;;)
        {
            std::size_t min_index;
            for (std::size_t i = 0; i < hull.size(); i++)
            {
                const std::size_t j = (i + 1) % hull.size();

                const glm::vec2 &p1 = hull[i], &p2 = hull[j];
                const glm::vec2 edge = p2 - p1;

                glm::vec2 normal = glm::normalize(glm::vec2(edge.y, -edge.x));
                float dist = glm::dot(normal, p1);
                if (dist < 0.f)
                {
                    dist *= -1.f;
                    normal *= -1.f;
                }
                if (dist < min_dist)
                {
                    min_dist = dist;
                    min_index = j;
                    mtv = normal;
                }
            }
            if (mtv.x == 0.f && mtv.y == 0.f)
                return {};

            const glm::vec2 support = sh1.support_point(mtv) - sh2.support_point(-mtv);
            const float sup_dist = glm::dot(mtv, support);
            const float diff = std::abs(sup_dist - min_dist);
            if (diff <= EPA_EPSILON)
                break;
            hull.insert(hull.begin() + (long)min_index, support);
            min_dist = FLT_MAX;
        }

        mtv *= min_dist;
        if (mtv.x == 0.f && mtv.y == 0.f)
            return {};
        return mtv;
    }

    std::pair<glm::vec2, glm::vec2> contact_points(const shape2D &sh1,
                                                   const shape2D &sh2,
                                                   const glm::vec2 &mtv)
    {
        PERF_FUNCTION()
        const glm::vec2 sup1 = sh1.support_point(mtv),
                        sup2 = sh2.support_point(-mtv);
        const float d1 = glm::length2(sh2.closest_direction_from(sup1 - mtv)),
                    d2 = glm::length2(sh1.closest_direction_from(sup2 + mtv));
        if (d1 < d2)
            return std::make_pair(sup1, sup1 - mtv);
        return std::make_pair(sup2 + mtv, sup2);
    }

    bool may_intersect(const shape2D &sh1, const shape2D &sh2)
    {
        return intersect(sh1.bounding_box(), sh2.bounding_box());
    }
    bool intersect(const aabb2D &bb1, const aabb2D &bb2)
    {
        const glm::vec2 df1 = bb2.min() - bb1.max(),
                        df2 = bb1.min() - bb2.max();
        if (df1.x > 0.f || df1.y > 0.f)
            return false;
        if (df2.x > 0.f || df2.y > 0.f)
            return false;
        return true;
    }

    bool intersect(const circle &c1, const circle &c2)
    {
        const float R = c1.radius() + c2.radius();
        return glm::distance2(c1.centroid(), c2.centroid()) < R * R;
    }
    glm::vec2 mtv(const circle &c1, const circle &c2)
    {
        const glm::vec2 dir = c1.centroid() - c2.centroid();
        return dir - (c1.radius() + c2.radius()) * glm::normalize(dir);
    }
    std::pair<glm::vec2, glm::vec2> contact_points(const circle &c1, const circle &c2)
    {
        const glm::vec2 dir = glm::normalize(c1.centroid() - c2.centroid());
        return std::make_pair(c1.centroid() - dir * c1.radius(), c2.centroid() + dir * c2.radius());
    }
}