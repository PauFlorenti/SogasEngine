#include <input/windows_keyboard_interface.h>

namespace sogas
{
namespace input
{
void WindowsKeyboardInterface::fetch_data(KeyBoard& keyboard)
{
    for (int i = 0; i < NUM_KEYBOARD_KEYS; i++)
    {
        const bool pressed = keys[i];
        keyboard.keys[i].set_value(pressed ? 1.f : 0.f);
    }
}

void WindowsKeyboardInterface::processMsg(HWND /*hWnd*/, UINT message, WPARAM wParam, LPARAM /*lParam*/)
{
    switch (message)
    {
        case WM_SYSKEYDOWN:
        case WM_KEYDOWN:
        {
            keys[(int)wParam] = true;
            break;
        }
        case WM_SYSKEYUP:
        case WM_KEYUP:
        {
            keys[(int)wParam] = false;
            break;
        }
    }
}
} // namespace input
} // namespace sogas
