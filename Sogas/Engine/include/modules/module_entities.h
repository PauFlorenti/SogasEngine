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
    EntityModule(const std::string& name)
    : IModule(name){};
    ~EntityModule() = default;

  protected:
    bool start() override;
    void stop() override;
    void update(f32 /*delta_time*/) override;
    void render() override{};
    void render_ui() override{};
    void render_debug() override{};

  private:
    std::vector<std::shared_ptr<engine::handle::HandleManager>> managers_to_update;
    //std::vector<std::shared_ptr<HandleManager>> managers_to_render_debug;
};
} // namespace modules
} // namespace sogas
