#include "geo/pch.hpp"
#include "geo/intersection.hpp"
#include "geo/polygon.hpp"
#include "perf/perf.hpp"
#include "dbg/log.hpp"

#define EPA_EPSILON 1.e-3f

namespace geo
{
    static float cross(const glm::vec2 &v1, const glm::vec2 &v2) { return v1.x * v2.y - v1.y * v2.x; }
    static glm::vec2 triple_cross(const glm::vec2 &v1, const glm::vec2 &v2, const glm::vec2 &v3)
    {
        const float crs = cross(v1, v2);
        return glm::vec2(-v3.y * crs, v3.x * crs);
    }

    static void line_case(const std::vector<glm::vec2> &simplex, glm::vec2 &dir)
    {
        const glm::vec2 AB = simplex[0] - simplex[1], AO = -simplex[1];
        dir = triple_cross(AB, AO, AB);
    }

    static bool triangle_case(std::vector<glm::vec2> &simplex, glm::vec2 &dir)
    {
        const glm::vec2 AB = simplex[1] - simplex[2], AC = simplex[0] - simplex[2], AO = -simplex[2];
        const glm::vec2 ABperp = triple_cross(AC, AB, AB);
        if (glm::dot(ABperp, AO) >= 0.f)
        {
            simplex.erase(simplex.begin());
            dir = ABperp;
            return false;
        }
        const glm::vec2 ACperp = triple_cross(AB, AC, AC);
        if (glm::dot(ACperp, AO) >= 0.f)
        {
            simplex.erase(simplex.begin() + 1);
            dir = ACperp;
            return false;
        }
        return true;
    }

    bool gjk(const shape2D &sh1, const shape2D &sh2, std::vector<glm::vec2> &simplex)
    {
        PERF_FUNCTION()
        DBG_ASSERT_WARN(!dynamic_cast<const circle *>(&sh1) || !dynamic_cast<const circle *>(&sh1), "Using gjk algorithm to check if two circles are intersecting, which is overkill")
        glm::vec2 dir = sh2.centroid() - sh1.centroid();
        const glm::vec2 supp = sh1.support_point(dir) - sh2.support_point(-dir);
        simplex.push_back(supp);
        dir = -supp;

        for (;;)
        {
            const glm::vec2 A = sh1.support_point(dir) - sh2.support_point(-dir);
            if (glm::dot(A, dir) <= 0.f)
                return false;
            simplex.push_back(A);
            if (simplex.size() == 2)
                line_case(simplex, dir);
            else if (triangle_case(simplex, dir))
                return true;
        }
    }

    bool epa(const shape2D &sh1, const shape2D &sh2, std::vector<glm::vec2> &simplex, glm::vec2 &mtv)
    {
        PERF_FUNCTION()
        DBG_ASSERT_WARN(polygon(simplex).contains_origin(), "Simplex passed to EPA algorithm does not contain the origin!\nx1: {0}, y1: {1}\nx2: {2}, y2: {3}\nx3: {4}, y3: {5}", simplex[0].x, simplex[0].y, simplex[1].x, simplex[1].y, simplex[2].x, simplex[2].y)
        float min_dist = FLT_MAX;
        mtv = {0.f, 0.f};
        for (;;)
        {
            std::size_t min_index;
            for (std::size_t i = 0; i < simplex.size(); i++)
            {
                const std::size_t j = (i + 1) % simplex.size();

                const glm::vec2 &p1 = simplex[i], &p2 = simplex[j];
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
                return false;

            const glm::vec2 support = sh1.support_point(mtv) - sh2.support_point(-mtv);
            const float sup_dist = glm::dot(mtv, support);
            const float diff = std::abs(sup_dist - min_dist);
            if (diff <= EPA_EPSILON)
                break;
            simplex.insert(simplex.begin() + (long)min_index, support);
            min_dist = FLT_MAX;
        }

        mtv *= min_dist;
        return mtv.x != 0.f || mtv.y != 0.f;
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