#pragma once

#ifdef _WIN64
#include <Windows.h>
#endif

using json = nlohmann::json;

#ifdef _WIN64
namespace sogas::engine::platform
{
class Window;

using window_proc   = LRESULT (*)(HWND, UINT, WPARAM, LPARAM);
using window_handle = HWND;
using Window_id     = u32;

struct window_init_info
{
    const char*   caption{nullptr};
    window_handle window_handle{nullptr}; // parent
    window_proc   window_proc{nullptr};
    i32           left{0};
    i32           top{0};
    i32           width{1280};
    i32           height{720};
};

// Window stuff ...
const Window_id create_window(const window_init_info* window_init_info = nullptr);
bool            peek_message();
void            remove_window(Window_id id);

void      resize_window(const Window_id id, const u32 width, const u32 height);
void      get_window_size(const Window_id id, u32& width, u32& height);
const u32 get_window_width(const Window_id id);
const u32 get_window_height(const Window_id id);
void      set_window_fullscreen(const Window_id id, const bool is_fullscreen);
bool      is_window_fullscreen(const Window_id);

// String functions
constexpr auto char_buffer_size = 1024 * 4;
i32            string_format(char* dest, const char* format, ...);
i32            string_format(char* dest, const char* format, va_list va_args);

// File stuff
json load_json(const std::string& filename);
} // namespace sogas::engine::platform

#else
#error "No other platform is implemented."
#endif // _WIN64
