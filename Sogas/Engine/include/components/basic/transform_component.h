#pragma once

#include <components/base_component.h>

#include <engine/transform.h>

namespace sogas
{
class TransformComponent : public Transform, public BaseComponent
{
  public:
    void update(f32 /*delta_time*/){};
    void on_entity_created(){};
    void load(const nlohmann::json& j, EntityParser& context);
    void render_debug_menu();
};
} // namespace sogas
