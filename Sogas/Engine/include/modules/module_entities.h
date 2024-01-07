#pragma once

#include <handle/handle_manager.h>
#include <modules/module.h>

namespace sogas
{
namespace modules
{
class EntityModule : public IModule
{
  public:
    EntityModule() = delete;
    EntityModule(const std::string& name) : IModule(name){};
    ~EntityModule() = default;

  protected:
    bool start() override;
    void stop() override;
    void update(f32 /*delta_time*/) override;
    void render() override{};
    void render_ui() override{};
    void render_debug(pinut::resources::CommandBuffer*) override;
    void render_debug_menu() override;
    void resize_window(u32, u32) override{};

  private:
    std::vector<HandleManager*> managers_to_update;
    std::vector<HandleManager*> managers_to_render_debug;
};

extern Handle get_entity_by_name(const std::string& name);
} // namespace modules
} // namespace sogas
