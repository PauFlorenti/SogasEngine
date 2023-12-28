#include "pch.hpp"

#include <components/basic/pau_component.h>
#include <components/basic/transform_component.h>

namespace sogas
{
DECLARE_OBJECT_MANAGER("pau", PauComponent);
void PauComponent::update(f32 delta_time)
{
    TransformComponent* transform = get<TransformComponent>();

    if (!transform)
    {
        return;
    }

    f32 yaw, pitch;
    transform->get_euler_angles(&yaw, &pitch);

    const auto new_yaw = yaw + speed * delta_time;
    transform->set_euler_angles(new_yaw, pitch, 0);
}
} // namespace sogas
