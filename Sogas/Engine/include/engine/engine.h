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

    void init();
    void run();
    void shutdown();

  private:
    void do_frame();

    modules::ModuleManager module_manager;
};
} // namespace engine
} // namespace sogas
