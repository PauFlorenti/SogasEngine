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
    PINFO("Start input module");
    return true;
}

void InputModule::stop()
{
    PINFO("Stop input module");
}

void InputModule::update(f32 /*delta_time*/)
{
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
