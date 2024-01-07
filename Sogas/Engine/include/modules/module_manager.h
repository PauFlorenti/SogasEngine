#pragma once

namespace pinut::resources
{
class CommandBuffer;
}

namespace sogas
{
namespace modules
{
class IModule;

using ModulesVector = std::vector<std::shared_ptr<IModule>>;

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
    void render_debug(pinut::resources::CommandBuffer* cmd);
    void render_debug_menu(const pinut::resources::CommandBuffer& cmd);
    void resize_window(u32 width, u32 height);

    void register_module(std::shared_ptr<IModule> module);
    void register_game_module(std::shared_ptr<IModule> module);

    std::shared_ptr<IModule> get_module(const std::string& name);

  private:
    void start_modules(const ModulesVector& modules);
    void stop_modules(const ModulesVector& modules);

    void parse_module_config(const std::string& config_file);

    ModulesVector services;
    ModulesVector update_modules;
    ModulesVector render_modules;

    std::map<std::string, std::shared_ptr<IModule>> registered_modules;
};
} // namespace modules
} // namespace sogas
