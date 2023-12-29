#include "pch.hpp"

#include <components/basic/camera_component.h>
#include <components/basic/transform_component.h>

namespace sogas
{
DECLARE_OBJECT_MANAGER("camera", CameraComponent);

void CameraComponent::update(f32 /*delta_time*/)
{
    TransformComponent* transform = get<TransformComponent>();
    ASSERT(transform);

    look_at(transform->get_position(), transform->get_forward(), transform->get_up());
}

void CameraComponent::load(const json& j, EntityParser& /*context*/)
{
    fov   = glm::radians(j.value("fov", fov));
    z_min = j.value("zmin", z_min);
    z_max = j.value("zmax", z_max);

    // TODO calculate aspect ratio.
    set_projection_parameters(fov, 1280.0f / 720.0f, z_min, z_max);
}
} // namespace sogas
