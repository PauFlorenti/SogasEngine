#include "pch.hpp"

#include <modules/module_input.h>

namespace sogas
{
namespace modules
{
namespace input
{
bool InputModule::start()
{
    printf("Start input module");
    return true;
}

void InputModule::stop()
{
    printf("Stop input module");
}

void InputModule::update(f32 /*delta_time*/)
{
    printf("Input module is being updated!");
}

void InputModule::render()
{
}

void InputModule::render_ui()
{
}

void InputModule::render_debug()
{
}
} // namespace input
} // namespace modules
} // namespace sogas
