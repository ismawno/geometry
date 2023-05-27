#ifndef SHAPE2D_HPP
#define SHAPE2D_HPP

#include "glm/vec2.hpp"
#include "geo/aabb2D.hpp"

namespace geo
{
class shape2D
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

  protected:
    glm::vec2 m_centroid;
    float m_angle = 0.f;
    aabb2D m_aabb;

    virtual void update() = 0;

#ifdef HAS_YAML_CPP
    virtual void write(YAML::Emitter &out) const;
    virtual YAML::Node encode() const;
    virtual bool decode(const YAML::Node &node);
#endif

  private:
    bool m_updating = false;

#ifdef HAS_YAML_CPP
    friend YAML::Emitter &operator<<(YAML::Emitter &, const shape2D &);
    friend struct YAML::convert<shape2D>;
#endif
};

#ifdef HAS_YAML_CPP
YAML::Emitter &operator<<(YAML::Emitter &out, const shape2D &sh);
#endif
} // namespace geo

#ifdef HAS_YAML_CPP
namespace YAML
{
template <> struct convert<geo::shape2D>
{
    static Node encode(const geo::shape2D &sh);
    static bool decode(const Node &node, geo::shape2D &sh);
};
} // namespace YAML
#endif

#endif