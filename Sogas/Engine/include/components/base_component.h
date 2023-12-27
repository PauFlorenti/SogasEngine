#pragma once

namespace sogas
{
struct EntityParser;
class BaseComponent
{
  public:
    void update(f32 /*delta_time*/){};
    void on_entity_created(){};
    void load(const nlohmann::json& /*json_file*/, EntityParser& /*scene*/){};
};
} // namespace sogas
