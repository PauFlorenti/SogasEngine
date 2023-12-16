#pragma once

#include <input/button.h>
#include <input/keyboard.h>

namespace sogas
{
namespace input
{
class WindowsKeyboardInterface
{
  public:
    void fetch_data(KeyBoard& keyboard);
    void processMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

  private:
    std::bitset<NUM_KEYBOARD_KEYS> keys;
};
} // namespace input
} // namespace sogas
