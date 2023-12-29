#pragma once

#include <components/base_component.h>
#include <engine/camera.h>
#include <entity/entity.h>

namespace sogas
{
class CameraComponent : public Camera, BaseComponent
{
    DECLARE_SIBILING_ACCESS()

  public:
    void update(f32 delta_time);
    void load(const json& j, EntityParser& context);
    void on_entity_created(){};
};
} // namespace sogas
