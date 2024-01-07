#include "pch.hpp"

#include <modules/module_input.h>

namespace sogas
{
namespace modules
{
const sogas::input::Button& InputModule::get_button(const std::string& /*button_name*/) const
{
    // TODO Read from reference buttons.
    PWARN("get_button function not properly implemented yet.");
    return keyboard.keys[0];
}

const sogas::input::Button& InputModule::get_key(i32 key) const
{
    return keyboard.keys[key];
}

const sogas::input::Button& InputModule::get_mouse_button(sogas::input::MouseButton button) const
{
    return mouse.buttons[static_cast<u32>(button)];
}

const glm::vec2& InputModule::get_mouse_position() const
{
    return mouse.position;
}

// Amount moved since last frame.
const glm::vec2& InputModule::get_mouse_offset() const
{
    return mouse.offset;
}

sogas::input::WindowsKeyboardInterface* InputModule::get_keyboard_device()
{
    return &keyboard_device;
}

bool InputModule::start()
{
    PINFO("Start input module");
    return true;
}

void InputModule::stop()
{
    PINFO("Stop input module");
}

void InputModule::update(f32 delta_time)
{
    keyboard_device.fetch_data(keyboard);
    mouse_device.fetch_data(mouse);

    keyboard.update(delta_time);
    mouse.update(delta_time);
}

void InputModule::render()
{
}

void InputModule::render_debug_menu()
{
    // TODO implement input render debug menu.
}
} // namespace modules
} // namespace sogas
