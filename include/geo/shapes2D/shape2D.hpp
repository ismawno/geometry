#pragma once

#include <glm/vec2.hpp>
#include <glm/mat2x2.hpp>
#include "geo/shapes2D/aabb2D.hpp"

#include "kit/utility/transform.hpp"
#include "kit/serialization/yaml/serializer.hpp"

namespace geo
{
class shape2D : public kit::yaml::serializable, public kit::yaml::deserializable
{
  public:
    shape2D(const kit::transform2D<float> &transform);
    shape2D() = default;
    virtual ~shape2D() = default;

    virtual glm::vec2 support_point(const glm::vec2 &direction) const = 0;
    virtual bool contains_point(const glm::vec2 &p) const = 0;
    bool contains_origin() const;

    void begin_update();
    void end_update();

    const aabb2D &bounding_box() const;
    virtual void bound() = 0;

    virtual glm::vec2 closest_direction_from(const glm::vec2 &p) const = 0;

    const kit::transform2D<float> &transform() const;
    const glm::vec2 &centroid() const;
    void centroid(const glm::vec2 &centroid);

    void translate(const glm::vec2 &dpos);
    void rotate(float drotation);

    void position(const glm::vec2 &pos);
    void rotation(float angle);
    void origin(const glm::vec2 &origin);

    virtual float area() const = 0;
    virtual float inertia() const = 0;
    virtual bool is_convex() const = 0;

  protected:
    kit::transform2D<float> m_transform;
    glm::vec2 m_global_centroid;
    aabb2D m_aabb;

    void update();
    virtual void on_shape_transform_update(const glm::mat3 &transform);

  private:
    bool m_pushing_update = false;
};

} // namespace geo
