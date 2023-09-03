#include "pch.hpp"

#include <fstream>

#include <platform/platform.h>

#ifdef _WIN64
namespace
{
using namespace sogas::engine::platform;

// Called every time the application receives a message
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LONG_PTR long_ptr{GetWindowLongPtr(hWnd, 0)};
    return long_ptr ?
             ((window_proc)long_ptr)(hWnd, message, wParam, lParam) :
             DefWindowProc(hWnd, message, wParam, lParam);
}

struct Window_info
{
    const char* caption{nullptr};
    RECT        client_area{0, 0, 1920, 1080};
    RECT        fullscreen_area{};
    POINT       top_left{0, 0};
    HWND        handle{nullptr};
    DWORD       style{0};
    bool        is_fullscreen{false};
};

std::vector<Window_info> windows; // Holds info for the open windows.
std::vector<Window_id>   available_windows; // If there is any empty slot in the windows vector, this will hold its id.

static const Window_id add_window(Window_info info)
{
    u32 id{INVALID_ID};
    if (available_windows.empty())
    {
        id = static_cast<u32>(windows.size());
        windows.emplace_back(info);
    }
    else
    {
        id = available_windows.back();
        available_windows.pop_back();
        windows[id] = info;
    }

    return id;
}

static Window_info& get_from_id(Window_id id)
{
    return windows.at(id);
}

static void remove_from_windows(Window_id id)
{
    ASSERT(id < windows.size());
    available_windows.emplace_back(id);
}

static void resize_window(const Window_info& info, const RECT& rect)
{
    RECT window_rect{rect};
    AdjustWindowRect(&window_rect, info.style, true);

    const i32 width{window_rect.right - window_rect.left};
    const i32 height{window_rect.bottom - window_rect.top};

    MoveWindow(info.handle, info.top_left.x, info.top_left.y, width, height, true);
}
} // namespace
#endif

namespace sogas::engine::platform
{
#ifdef _WIN64

const Window_id create_window(const window_init_info* window_init_info)
{
    window_proc   callback = window_init_info ? window_init_info->window_proc : nullptr;
    window_handle parent   = window_init_info ? window_init_info->window_handle : nullptr;

    // Register class
    WNDCLASSEX wcex{};
    ZeroMemory(&wcex, sizeof(wcex));
    wcex.cbSize        = sizeof(WNDCLASSEX);
    wcex.style         = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc   = WndProc;
    wcex.cbClsExtra    = 0;
    wcex.cbWndExtra    = callback ? sizeof(callback) : 0;
    wcex.hInstance     = 0;
    wcex.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName  = NULL;
    wcex.lpszClassName = "SogasEngineClass";
    wcex.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    RegisterClassEx(&wcex);

#if _DEBUG
    SetLastError(0);
#endif

    // Create window
    Window_info info{};
    info.caption = (window_init_info && window_init_info->caption) ? window_init_info->caption : "Sogas Engine";

    info.style = WS_VISIBLE;
    info.style |= parent ? WS_CHILD : WS_OVERLAPPEDWINDOW;

    // Desired window client size
    const i32 left   = window_init_info ? window_init_info->left : info.client_area.left;
    const i32 top    = window_init_info ? window_init_info->top : info.client_area.top;
    const i32 width  = window_init_info ? window_init_info->width : info.client_area.right - info.client_area.left;
    const i32 height = window_init_info ? window_init_info->height : info.client_area.bottom - info.client_area.top;

    info.client_area = {left, top, width, height};

    // Window size
    RECT rc = {};
    AdjustWindowRect(&rc, info.style, FALSE);

    // Calculate window size to accomodate desired client window size.
    const auto window_x      = left + rc.left;
    const auto window_y      = top + rc.top;
    const auto window_width  = width + rc.right - rc.left;
    const auto window_height = height + rc.bottom - rc.top;

    HWND hWnd = CreateWindow(
      "SogasEngineClass",
      info.caption,
      info.style,
      window_x,
      window_y,
      window_width,
      window_height,
      parent,
      NULL,
      NULL,
      NULL);

    if (!hWnd)
    {
        ASSERT(GetLastError() != 0);
#if _DEBUG
        SetLastError(0);
#endif
        FATAL("No valid window handle created!\n");
        return INVALID_ID;
    }

    //AdjustWindowRect(&info.client_area, info.style, info.is_fullscreen ? false : true);

    if (callback)
        SetWindowLongPtr(hWnd, 0, (LONG_PTR)callback);

    ShowWindow(hWnd, SW_SHOW);

    info.handle = std::move(hWnd);

    return add_window(std::move(info));
}

bool peek_message()
{
    MSG msg{0};
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
            return false;
    }

    return true;
}

void remove_window(Window_id id)
{
    DestroyWindow(static_cast<platform::window_handle>(get_from_id(id).handle));
    remove_from_windows(id);
}

void resize_window(const Window_id id, const u32 width, const u32 height)
{
    auto& info{get_from_id(id)};

    auto& area{info.is_fullscreen ? info.fullscreen_area : info.client_area};
    area.bottom = area.top + height;
    area.right  = area.left + width;

    resize_window(info, area);
}

void get_window_size(const Window_id id, u32& width, u32& height)
{
    const auto& info = get_from_id(id);
    width            = info.client_area.right - info.client_area.left;
    height           = info.client_area.bottom - info.client_area.top;
}

const u32 get_window_width(const Window_id id)
{
    const auto& client_area = get_from_id(id).client_area;
    return client_area.left - client_area.right;
}

const u32 get_window_height(const Window_id id)
{
    const auto& client_area = get_from_id(id).client_area;
    return client_area.bottom - client_area.top;
}

void set_window_fullscreen(const Window_id id, bool is_fullscreen)
{
    auto& info = get_from_id(id);

    if (info.is_fullscreen == is_fullscreen)
        return;

    info.is_fullscreen = is_fullscreen;

    if (is_fullscreen)
    {
        // Save client area
        GetClientRect(info.handle, &info.client_area);
        RECT rect;
        GetWindowRect(info.handle, &rect);
        info.top_left.x = rect.left;
        info.top_left.y = rect.top;
        info.style      = 0;
        SetWindowLongPtr(info.handle, GWL_STYLE, info.style);
        ShowWindow(info.handle, SW_MAXIMIZE);
    }
    else
    {
        info.style = WS_VISIBLE | WS_OVERLAPPEDWINDOW;
        SetWindowLongPtr(info.handle, GWL_STYLE, info.style);
        resize_window(info, info.client_area);
        ShowWindow(info.handle, SW_SHOWNORMAL);
    }
}

bool is_window_fullscreen(const Window_id id)
{
    return get_from_id(id).is_fullscreen;
}

i32 string_format(char* dest, const char* format, ...)
{
    if (dest != nullptr)
    {
        va_list argptr;
        va_start(argptr, format);
        const auto number_written = string_format(dest, format, argptr);
        va_end(argptr);
        return number_written;
    }

    return -1;
}

i32 string_format(char* dest, const char* format, va_list va_args)
{
    if (dest != nullptr)
    {
        char       buffer[char_buffer_size];
        const auto number_written = _vsnprintf_s(buffer, sizeof(buffer), (size_t)char_buffer_size, format, va_args);
        memcpy(dest, buffer, number_written + 1);
        return number_written;
    }

    return -1;
}

json load_json(const std::string& filename)
{
    json j = {};
    while (true)
    {
        std::ifstream ifs(filename.c_str());

        if (!ifs.is_open())
        {
            ERROR("ERROR: Could not open json file %s.", filename.c_str());
            continue;
        }

#ifdef NDEBUG
        j = json::parse(ifs, nullptr, false);

        if (j.is_discarded())
        {
            ERROR("Json discarded\n");
            ifs.close();
            ERROR("ERROR: Could not open json file %s.", filename.c_str());
            continue;
        }
#else
        j = json::parse(ifs);
#endif
        break;
    }

    return j;
}

#else
#error "Only win64 platform implemented at the moment."
#endif
} // namespace sogas::engine::platform
