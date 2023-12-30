#pragma once

#include <input/button.h>

namespace sogas
{
namespace input
{
enum class MouseButton
{
    LEFT = 0,
    MIDDLE,
    RIGHT,
    COUNT
};

constexpr i32 MOUSE_BUTTON_COUNT = static_cast<i32>(MouseButton::COUNT);

struct Mouse
{
    Button    buttons[3];
    glm::vec2 position    = glm::vec2(0.0f);
    glm::vec2 offset      = glm::vec2(0.0f);
    i32       wheel_steps = 0;

    void update(f32 delta_time)
    {
        for (auto& button : buttons)
        {
            button.update(delta_time);
        }
    }
};
} // namespace input
} // namespace sogas
