#pragma once

#include <input/button.h>

namespace sogas
{
namespace input
{
constexpr static i32 NUM_KEYBOARD_KEYS = 256;
struct KeyBoard
{
    Button keys[NUM_KEYBOARD_KEYS] = {};

    void update(f32 delta_time)
    {
        for (auto& k : keys)
        {
            k.update(delta_time);
        }
    }
};
} // namespace input
} // namespace sogas
