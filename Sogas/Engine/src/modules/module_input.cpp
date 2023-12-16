#include "pch.hpp"

#include <modules/module_input.h>

namespace sogas
{
namespace modules
{
namespace input
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

    keyboard.update(delta_time);
}

void InputModule::render()
{
}
} // namespace input
} // namespace modules
} // namespace sogas
