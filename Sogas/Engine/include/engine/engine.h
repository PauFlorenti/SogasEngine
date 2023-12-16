#pragma once

#include <engine/camera.h>
#include <modules/module_manager.h>

namespace sogas
{
namespace modules
{
namespace input
{
class InputModule;
}
} // namespace modules
namespace engine
{
class Engine
{
  public:
    Engine()  = default;
    ~Engine() = default;

    Engine(Engine& other)  = delete;
    Engine(Engine&& other) = delete;

    static Engine& Get();

    void init();
    void run();
    void shutdown();
    void resize(u32 width, u32 height);

    std::shared_ptr<modules::input::InputModule> get_input();
    // TODO Same as below. Temporal.
    Camera& get_camera()
    {
        return engine_camera;
    }

  private:
    void do_frame();

    static Engine* engine_instance;

    // TODO This is temporal. Camera should be in the scene.
    Camera                 engine_camera;
    modules::ModuleManager module_manager;
};
} // namespace engine
} // namespace sogas
