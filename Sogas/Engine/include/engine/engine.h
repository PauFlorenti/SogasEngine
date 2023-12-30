#pragma once

#include <engine/camera.h>
#include <modules/module_manager.h>

namespace sogas
{
struct Clock;
class Mesh;
namespace modules
{
class InputModule;
class RendererModule;
} // namespace modules
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

    std::shared_ptr<modules::InputModule>    get_input();
    std::shared_ptr<modules::RendererModule> get_renderer();

    // TODO Same as below. Temporal.
    std::map<std::string, Mesh*>* get_meshes()
    {
        return &meshes;
    };

  private:
    void do_frame();

    static Engine* engine_instance;

    // TODO This is temporal. Camera should be in the scene as an entity.
    modules::ModuleManager       module_manager;
    std::map<std::string, Mesh*> meshes;
    
    Clock*                       clock = nullptr;
    f64                          delta_time{0};
    f64                          last_time{0};
};
} // namespace sogas
