#pragma once

#include <engine/camera.h>
#include <modules/module_manager.h>

#include <resources/mesh.h>

namespace sogas
{
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
    Camera& get_camera()
    {
        return engine_camera;
    }

    std::map<std::string, sogas::resources::Mesh*>* get_meshes()
    {
        return &meshes;
    };

  private:
    void do_frame();

    static Engine* engine_instance;

    // TODO This is temporal. Camera should be in the scene.
    Camera                                         engine_camera;
    modules::ModuleManager                         module_manager;
    std::map<std::string, sogas::resources::Mesh*> meshes;
};
} // namespace sogas
