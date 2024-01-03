#include "pch.hpp"

#include <components/basic/point_light_component.h>
#include <components/basic/transform_component.h>
#include <engine/primitives.h>
#include <render_device.h>
#include <resources/json_helper.h>

namespace sogas
{
DECLARE_OBJECT_MANAGER("point_light", PointLightComponent);
void PointLightComponent::load(const json& j, EntityParser& /*context*/)
{
    if (j.count("color"))
    {
        color = load_vec4(j["color"]);
    }

    radius    = j.value("radius", radius);
    enabled   = j.value("enabled", enabled);
    intensity = j.value("intensity", intensity);
}
void PointLightComponent::render_debug(pinut::resources::CommandBuffer* cmd)
{
    TransformComponent* transform_component = get<TransformComponent>();
    draw_wired_sphere(cmd,
                      glm::translate(glm::mat4(1.0f), transform_component->get_position()),
                      radius,
                      color);
}
} // namespace sogas
