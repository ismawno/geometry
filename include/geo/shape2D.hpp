#ifndef GEO_SHAPE2D_HPP
#define GEO_SHAPE2D_HPP

#include "glm/vec2.hpp"
#include "glm/mat2x2.hpp"
#include "geo/aabb2D.hpp"

#include "kit/interface/serialization.hpp"

namespace geo
{
class shape2D : public kit::serializable
{
  public:
    shape2D(const glm::vec2 &centroid = glm::vec2(0.f), float angle = 0.f);
    virtual ~shape2D() = default;

    virtual glm::vec2 support_point(const glm::vec2 &direction) const = 0;
    virtual bool contains_point(const glm::vec2 &p) const = 0;
    bool contains_origin() const;

    void begin_update();
    void end_update();

    const aabb2D &bounding_box() const;
    virtual glm::vec2 closest_direction_from(const glm::vec2 &p) const = 0;

    void translate(const glm::vec2 &dpos);
    const glm::vec2 &centroid() const;
    void centroid(const glm::vec2 &pos);

    virtual float area() const = 0;
    virtual float inertia() const = 0;
    virtual bool is_convex() const = 0;

    void rotate(float dangle);
    void rotation(float angle);
    float rotation() const;

#ifdef KIT_USE_YAML_CPP
    virtual YAML::Node encode() const override;
    virtual bool decode(const YAML::Node &node) override;
#endif

  protected:
    glm::vec2 m_centroid;
    float m_angle = 0.f;
    aabb2D m_aabb;

    virtual void update() = 0;

  private:
    bool m_pushing = false;
};

} // namespace geo

#endif