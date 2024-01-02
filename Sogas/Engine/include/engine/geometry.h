#pragma once

namespace sogas
{
enum class ContainmentType
{
    CONTAINS = 0,
    INTERSECTS,
    EXCLUDE
};

class BoundingSphere
{
  public:
    // clang-format off
    BoundingSphere() : center({0.0f}), radius(1.0f) {}

    BoundingSphere(const BoundingSphere&) = default;
    BoundingSphere& operator=(const BoundingSphere&) = default;

    BoundingSphere(BoundingSphere&&) = default;
    BoundingSphere& operator=(BoundingSphere&&) = default;
    // clang-format on

    ContainmentType contains(const glm::vec3& point) const;
    ContainmentType contains(const BoundingSphere& sphere) const;

    bool intersects(const BoundingSphere& sphere) const;
    bool intersects(const glm::vec3& origin, const glm::vec3& direction, f32& dist) const;

    // TODO Contained by frustrum.

    void render_debug();

    glm::vec3 center;
    f32       radius;
};

// TODO BoundingBox
} // namespace sogas
