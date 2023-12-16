#pragma once

namespace sogas
{
namespace input
{
struct Button
{
    bool pressed        = false;
    bool previous_value = false;
    f32  time_pressed   = 0.0f;

    void update(f32 delta_time)
    {
        if (pressed)
        {
            time_pressed += delta_time;
        }
        else
        {
            time_pressed = 0.0f;
        }
    }

    void set_value(bool new_value)
    {
        previous_value = pressed;
        pressed        = new_value;
    }

    bool is_pressed() const
    {
        return pressed;
    }

    bool was_pressed() const
    {
        return previous_value;
    }

    bool gets_pressed() const
    {
        return is_pressed() && !was_pressed();
    }

    bool gets_released() const
    {
        return !is_pressed() && was_pressed();
    }
};
} // namespace input
} // namespace engine
