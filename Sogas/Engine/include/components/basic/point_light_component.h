#pragma once

#include <components/base_component.h>
#include <entity/entity.h>

namespace sogas
{
class PointLightComponent : public BaseComponent
{
    DECLARE_SIBILING_ACCESS();
  public:
    void load(const json& j, EntityParser& context);
    void update(f32 delta_time);

    glm::vec4 color     = glm::vec4(1.0f);
    f32       intensity = 1.0f;
    f32       radius    = 1.0f;
    bool      enabled   = true;
    glm::vec3 position  = glm::vec3(0.0f); // TODO For when activating through function.
};
} // namespace sogas
