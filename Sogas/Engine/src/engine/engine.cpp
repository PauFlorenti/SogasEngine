
#include "pch.hpp"

#include <chrono>
#include <engine/clock.h>
#include <engine/engine.h>
#include <modules/module_boot.h>
#include <modules/module_entities.h>
#include <modules/module_input.h>
#include <modules/module_renderer.h>
#include <platform/platform.h>
#include <resources/mesh.h>

LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace
{
using namespace sogas::platform;
Window_id window;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
#if _DEBUG
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
    {
        return true;
    }
#endif

    PAINTSTRUCT ps;
    HDC         hdc;

    switch (message)
    {
        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        case WM_SIZE:
            RECT r;
            GetClientRect(hWnd, &r);
            sogas::Engine::Get().resize(r.right - r.left, r.bottom - r.top);
            break;

        case WM_SYSCHAR:
            if (wParam == VK_RETURN && (HIWORD(lParam) & KF_ALTDOWN))
            {
                set_window_fullscreen(window, !is_window_fullscreen(window));
                return 0;
            }
            break;
        case WM_SYSCOMMAND:
        {
            if (wParam == SC_KEYMENU && (lParam >> 16) <= 0)
                return 0;
            return DefWindowProc(hWnd, message, wParam, lParam);
        }

        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        {
            auto keyboard_device = sogas::Engine::Get().get_input()->get_keyboard_device();
            keyboard_device->processMsg(hWnd, message, wParam, lParam);
            break;
        }
        case WM_KEYUP:
        {
            auto keyboard_device = sogas::Engine::Get().get_input()->get_keyboard_device();
            keyboard_device->processMsg(hWnd, message, wParam, lParam);
            if (wParam == VK_ESCAPE)
                PostQuitMessage(0);
            break;
        }
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MOUSEMOVE:
        {
            auto mouse_device = sogas::Engine::Get().get_input()->get_mouse_device();
            mouse_device->processMsg(hWnd, message, wParam, lParam);
        }
        case WM_MOUSEWHEEL:
        case WM_INPUT:
            break;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}
} // namespace

namespace sogas
{
Engine*    Engine::engine_instance = nullptr;
static f64 time_stamp;

Engine& Engine::Get()
{
    return *engine_instance;
}

void Engine::init()
{
    PDEBUG("Initializing engine!");

    engine_instance = this;

    // TODO If more than one main window ... (should not happen) close the program only when the last is closed.
    // Should have a way to check for that ... maybe a vector or array of windows.

    platform::window_init_info init_info{"Sogas Engine", nullptr, WndProc, 100, 100, 1280, 720};
    window = platform::create_window(&init_info);

    // TODO register modules

    // Given modules by the engine
    // Log module
    // Input module
    // Physics module
    // Render module
    // Clock module ??
    // Entity module
    module_manager.register_module(std::make_shared<modules::EntityModule>("entity"));
    module_manager.register_module(std::make_shared<modules::InputModule>("input"));
    module_manager.register_module(
      std::make_shared<modules::RendererModule>("renderer", platform::get_window_handle(window)));
    module_manager.register_module(std::make_shared<modules::BootModule>("boot"));

    // TODO register standalone game components

    module_manager.boot();

    clock = new Clock();
    start_clock(clock);
    update_clock(clock);
}

void Engine::run()
{
    // TODO Should clock be computed here and passed to the modules of the engine??
    // I believe that the delta time should be computed by the Engine in maybe a clock or time module?

    // Iterate through modules

    // Platform check for messages
    // If no messages, do frame

    bool should_quit{false};
    while (!should_quit)
    {
        should_quit = platform::peek_message();

        update_clock(clock);
        f64 current_time = clock->elapsed_time;
        delta_time       = (current_time - last_time) / 10000;

        if (auto input = get_input())
        {
            if (input->get_key(VK_F1).gets_released())
            {
                auto renderer         = get_renderer();
                bool enable_wireframe = renderer->get_wireframe_enabled();
                renderer->set_wireframe(!enable_wireframe);
            }
        }

        do_frame();
    }
}

void Engine::shutdown()
{
    stop_clock(clock);
    delete clock;

    PDEBUG("Shuting down engine!");
    platform::remove_window(window);

    // Resources should be removed before renderer.
    for (auto mesh : meshes)
    {
        mesh.second->destroy();
    }
    meshes.clear();

    module_manager.clear();
}

void Engine::resize(u32 width, u32 height)
{
    module_manager.resize_window(width, height);
}

std::shared_ptr<modules::InputModule> Engine::get_input()
{
    return std::static_pointer_cast<modules::InputModule>(module_manager.get_module("input"));
}

std::shared_ptr<modules::RendererModule> Engine::get_renderer()
{
    return std::static_pointer_cast<modules::RendererModule>(module_manager.get_module("renderer"));
}

void Engine::do_frame()
{
    // Calculate delta time
    // Modules update here
    // Update Input
    // Update physics
    // Update graphics
    module_manager.update((f32)delta_time);
    module_manager.render();
}
} // namespace sogas
