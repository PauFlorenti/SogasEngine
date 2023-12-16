#pragma once

#include <input/windows_keyboard_interface.h>
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
    InputModule(const std::string& name) : IModule(name){};
    ~InputModule() = default;

    const sogas::input::Button&             get_button(const std::string& button_name) const;
    const sogas::input::Button&             get_key(i32 key) const;
    sogas::input::WindowsKeyboardInterface* get_keyboard_device();

  protected:
    bool start() override;
    void stop() override;
    void update(f32 delta_time) override;
    void render() override;
    void render_ui() override{};
    void render_debug() override{};
    void resize_window(u32, u32) override{};

  private:
    sogas::input::KeyBoard                 keyboard;
    sogas::input::WindowsKeyboardInterface keyboard_device;
};
} // namespace input
} // namespace modules
} // namespace sogas
