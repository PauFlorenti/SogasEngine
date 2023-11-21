#pragma once

#include <modules/module_manager.h>

namespace sogas
{
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

  private:
    void do_frame();

    static Engine* engine_instance;

    modules::ModuleManager module_manager;
};
} // namespace engine
} // namespace sogas
