#pragma once

#include <components/base_component.h>
#include <entity/entity.h>

namespace sogas
{
class PauComponent : public BaseComponent
{
    DECLARE_SIBILING_ACCESS()

    f32 speed = 1.0f;

  public:
    void update(f32 delta_time);
    void on_entity_created(){};
    void load(const nlohmann::json& /*json_file*/, EntityParser& /*scene*/){};
};
} // namespace sogas
