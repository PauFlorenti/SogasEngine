#include "pch.hpp"

#include <input/devices/windows_mouse_interface.h>

namespace sogas
{
namespace input
{
void WindowsMouseInterface::fetch_data(Mouse& mouse)
{
    for (i32 i = 0; i < MOUSE_BUTTON_COUNT; i++)
    {
        mouse.buttons[i].set_value(buttons[i] ? 1.0 : 0.0);
    }

    mouse.position    = current_position;
    mouse.offset      = offset;
    mouse.wheel_steps = wheel_steps;

    offset      = glm::vec2(0.0f);
    wheel_steps = 0;
}

void WindowsMouseInterface::processMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_LBUTTONDOWN:
        {
            buttons[static_cast<u32>(MouseButton::LEFT)] = true;
            SetCapture(hWnd);
            break;
        }
        case WM_LBUTTONUP:
        {
            buttons[static_cast<u32>(MouseButton::LEFT)] = false;
            ReleaseCapture();
            break;
        }
        case WM_MBUTTONDOWN:
        {
            buttons[static_cast<u32>(MouseButton::MIDDLE)] = true;
            SetCapture(hWnd);
            break;
        }
        case WM_MBUTTONUP:
        {
            buttons[static_cast<u32>(MouseButton::MIDDLE)] = false;
            ReleaseCapture();
            break;
        }
        case WM_RBUTTONDOWN:
        {
            buttons[static_cast<u32>(MouseButton::RIGHT)] = true;
            SetCapture(hWnd);
            break;
        }
        case WM_RBUTTONUP:
        {
            buttons[static_cast<u32>(MouseButton::RIGHT)] = false;
            ReleaseCapture();
            break;
        }
        case WM_MOUSEMOVE:
        {
            glm::vec2 new_position = glm::vec2(0.0f);
            new_position.x         = static_cast<f32>(GET_X_LPARAM(lParam));
            new_position.y         = static_cast<f32>(GET_Y_LPARAM(lParam));
            offset                 = new_position - current_position;
            current_position       = new_position;
            break;
        }
        case WM_MOUSEWHEEL:
        {
            wheel_steps = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
            break;
        }
    }
}
} // namespace input
} // namespace sogas
