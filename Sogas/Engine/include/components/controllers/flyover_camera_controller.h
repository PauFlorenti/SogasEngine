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
    f32       speed_factor = 10.0f;
    f32       sensibility  = 10.0f;
    bool      enabled      = true;
};
} // namespace sogas
