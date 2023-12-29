#pragma once

#include <components/base_component.h>
#include <engine/camera.h>
#include <entity/entity.h>

namespace sogas
{
class FlyoverCameraController : public BaseComponent
{
    DECLARE_SIBILING_ACCESS();

  public:
    void update(f32 delta_time);
    void load(const json& j, EntityParser& context);

  private:
    f32       speed_factor = 1.0f;
    glm::vec3 speed        = glm::vec3(0.0f);
    bool      enabled      = true;
};
} // namespace sogas
