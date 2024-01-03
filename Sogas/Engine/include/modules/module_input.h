#pragma once

#include <input/devices/windows_keyboard_interface.h>
#include <input/devices/windows_mouse_interface.h>
#include <input/mouse.h>
#include <modules/module.h>

namespace sogas
{
namespace modules
{
class InputModule final : public IModule
{
  public:
    InputModule() = delete;
    InputModule(const std::string& name) : IModule(name){};
    ~InputModule() = default;

    const sogas::input::Button& get_button(const std::string& button_name) const;
    const sogas::input::Button& get_key(i32 key) const;
    const sogas::input::Button& get_mouse_button(sogas::input::MouseButton button) const;
    const glm::vec2&            get_mouse_position() const;
    const glm::vec2&            get_mouse_offset() const;
    sogas::input::WindowsKeyboardInterface* get_keyboard_device();
    sogas::input::WindowsMouseInterface*    get_mouse_device()
    {
        return &mouse_device;
    }

  protected:
    bool start() override;
    void stop() override;
    void update(f32 delta_time) override;
    void render() override;
    void render_ui() override{};
    void render_debug(pinut::resources::CommandBuffer*) override{};
    void resize_window(u32, u32) override{};

  private:
    sogas::input::KeyBoard keyboard;
    sogas::input::Mouse    mouse;
    // TODO list of devices ... neded an interface to fetch data for different types of devices.
    sogas::input::WindowsKeyboardInterface keyboard_device;
    sogas::input::WindowsMouseInterface    mouse_device;
};
} // namespace modules
} // namespace sogas
