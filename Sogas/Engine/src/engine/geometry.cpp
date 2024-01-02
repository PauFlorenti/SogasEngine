#include "pch.hpp"

#include <engine/geometry.h>

namespace sogas
{
ContainmentType BoundingSphere::contains(const glm::vec3& point) const
{
    const auto length        = glm::length(point - center);
    const auto radius_length = glm::length(radius);

    return length <= radius_length ? ContainmentType::CONTAINS : ContainmentType::EXCLUDE;
}

ContainmentType BoundingSphere::contains(const BoundingSphere& /*sphere*/) const
{
    PWARN("Function not implemented");
    return ContainmentType::CONTAINS;
}

bool BoundingSphere::intersects(const BoundingSphere& sphere) const
{
    const auto distance = glm::length(center - sphere.center);
    return distance < radius ? true : false;
}

bool BoundingSphere::intersects(const glm::vec3& /*origin*/,
                                const glm::vec3& /*direction*/,
                                f32&             /*dist*/) const
{
    PWARN("Function not implemented");
    return false;
}

// TODO Contained by frustrum.

void render_debug()
{
}
} // namespace sogas
