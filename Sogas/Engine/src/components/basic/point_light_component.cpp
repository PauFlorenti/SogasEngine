#include "pch.hpp"

#include <components/basic/point_light_component.h>
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

static f32 aux = 0.0f;

void PointLightComponent::update(f32 /*delta_time*/)
{
    //aux += delta_time * 10.0f;
    //color.x = sinf(aux);
}
} // namespace sogas
