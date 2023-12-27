#pragma once

#include <modules/module.h>

namespace sogas
{
namespace modules
{
class BootModule : public IModule
{
  public:
    BootModule(const std::string& name) : IModule(name){};
    bool start() override;
    void stop() override {};
    void update(f32 /*delta_time*/) override {};
    void render() override {};
    void render_ui() override {};
    void render_debug() override {};
    void resize_window(u32, u32) override {};

  private:
    void load_scene(const std::string& filename);
};
} // namespace modules
} // namespace sogas
