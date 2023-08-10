#pragma once

namespace sogas
{
namespace engine
{
class Scene;
namespace components
{
class BaseComponent
{
  public:
    void update(f32 /*delta_time*/){};
    void on_entity_created(){};
    void load(const nlohmann::json& /*json_file*/, Scene& /*scene*/){};
};
} // namespace components
} // namespace engine
} // namespace sogas
