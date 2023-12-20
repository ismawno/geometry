#pragma once

#include "glm/vec2.hpp"
#include "glm/mat2x2.hpp"
#include "geo/aabb2D.hpp"

#include "kit/interface/serialization.hpp"
#include "kit/utility/transform.hpp"

namespace geo
{
class shape2D : public kit::serializable
{
  public:
    shape2D(const kit::transform2D &transform);
    shape2D() = default;
    virtual ~shape2D() = default;

    virtual glm::vec2 support_point(const glm::vec2 &direction) const = 0;
    virtual bool contains_point(const glm::vec2 &p) const = 0;
    bool contains_origin() const;

    void begin_update();
    void end_update();

    const aabb2D &bounding_box() const;
    virtual glm::vec2 closest_direction_from(const glm::vec2 &p) const = 0;

    const kit::transform2D &transform() const;
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

#ifdef KIT_USE_YAML_CPP
    virtual YAML::Node encode() const override;
    virtual bool decode(const YAML::Node &node) override;
#endif

  protected:
    kit::transform2D m_transform;
    glm::vec2 m_global_centroid;
    aabb2D m_aabb;

    void update();
    virtual void on_shape_transform_update(const glm::mat3 &transform);

  private:
    bool m_pushing_update = false;
};

} // namespace geo
