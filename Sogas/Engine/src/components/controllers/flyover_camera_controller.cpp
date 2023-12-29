#include "pch.hpp"

#include <components/basic/transform_component.h>
#include <components/controllers/flyover_camera_controller.h>
#include <engine/engine.h>
#include <modules/module_input.h>

namespace sogas
{
DECLARE_OBJECT_MANAGER("flyover_camera", FlyoverCameraController);

void FlyoverCameraController::update(f32 delta_time)
{
    if (!enabled)
    {
        return;
    }

    TransformComponent* transform = get<TransformComponent>();
    if (!transform)
    {
        return;
    }

    const auto delta_speed = delta_time * speed_factor;
    const auto position    = transform->get_position();
    const auto forward     = transform->get_forward();
    const auto right       = transform->get_right();
    const auto up          = glm::vec3(0.0f, 1.0f, 0.0f);

    // TODO rotation.

    if (auto input = Engine::Get().get_input())
    {
        if (input->get_key(0x57).is_pressed())
        {
            speed.z = 1.0f;
        }
        else if (input->get_key(0x53).is_pressed())
        {
            speed.z = -1.0f;
        }
        else if (input->get_key(0x41).is_pressed())
        {
            speed.x = -1.0f;
        }
        else if (input->get_key(0x44).is_pressed())
        {
            speed.x = 1.0f;
        }
        else if (input->get_key(0x5A).is_pressed())
        {
            speed.y = 1.0f;
        }
        else if (input->get_key(0x58).is_pressed())
        {
            speed.y = -1.0f;
        }
        else if (input->get_key(0x51).is_pressed())
        {
            // const auto view     = engine_camera.get_view();
            // glm::quat  rotation = glm::quat(view);
        }
        else if (input->get_key(0x52).is_pressed())
        {
        }

        glm::vec3 offset = glm::vec3(0.0f);
        offset += forward * speed.z * delta_speed;
        offset += right * speed.x * delta_speed;
        offset += up * speed.y * delta_speed;

        auto new_position = position + offset;
        transform->lookAt(new_position, new_position + forward, up);

        speed = glm::vec3(0.0f);
    }
}

void FlyoverCameraController::load(const json& j, EntityParser& /*context*/)
{
    if (j.count("speed") && j["speed"].is_number())
    {
        speed_factor = j.value("speed", speed_factor);
    }

    enabled = j.value("enabled", enabled);
}
} // namespace sogas
