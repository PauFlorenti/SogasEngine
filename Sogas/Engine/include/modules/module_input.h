#pragma once

#include <modules/module.h>

namespace sogas
{
namespace modules
{
namespace input
{
class InputModule final : public IModule
{
  public:
    InputModule() = delete;
    InputModule(const std::string& name)
    : IModule(name){};
    ~InputModule() = default;

  protected:
    bool start() override;
    void stop() override;
    void update(f32 delta_time) override;
    void render() override;
    void render_ui() override;
    void render_debug() override;
};
} // namespace input
} // namespace modules
} // namespace sogas