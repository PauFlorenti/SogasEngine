#pragma once

#include <input/mouse.h>

namespace sogas
{
namespace input
{
class WindowsMouseInterface
{
  public:
    void fetch_data(Mouse& mouse);
    void processMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

  private:
    std::bitset<MOUSE_BUTTON_COUNT> buttons;

    glm::vec2 current_position = glm::vec2(0.0f);
    glm::vec2 offset           = glm::vec2(0.0f);
    i32       wheel_steps      = 0;
};
} // namespace input
} // namespace sogas
