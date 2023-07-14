#pragma once

namespace sogas
{
namespace modules
{
class IModule;

using VectorModules = std::vector<std::shared_ptr<IModule>>;

class ModuleManager
{
  public:
    ModuleManager()                      = default;
    ModuleManager(ModuleManager& other)  = delete;
    ModuleManager(ModuleManager&& other) = delete;

    void boot();
    void clear();

    void update(f32 delta_time);
    void render();
    void render_ui();
    void render_debug();

    void register_module(std::shared_ptr<IModule> module);
    void register_game_module(std::shared_ptr<IModule> module);

  private:
    VectorModules services;
    VectorModules update_modules;
    VectorModules render_modules;

    std::map<std::string, std::shared_ptr<IModule>> registered_modules;
};
} // namespace modules
} // namespace sogas