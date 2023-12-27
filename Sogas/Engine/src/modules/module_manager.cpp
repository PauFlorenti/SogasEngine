
#include "pch.hpp"

#include <modules/module.h>
#include <modules/module_manager.h>
#include <platform/platform.h>

namespace sogas
{
namespace modules
{
void ModuleManager::boot()
{
    parse_module_config("../../Sogas/Engine/data/modules.json");
}

void ModuleManager::clear()
{
    stop_modules(services);
    services.clear();
}

void ModuleManager::update(f32 delta_time)
{
    for (const auto& module : update_modules)
    {
        if (module->get_is_active())
        {
            module->update(delta_time);
        }
    }
}

void ModuleManager::render()
{
    for (const auto& module : render_modules)
    {
        if (module->get_is_active())
        {
            module->render();
        }
    }
}

void ModuleManager::render_ui()
{
    for (const auto& module : render_modules)
    {
        if (module->get_is_active())
        {
            module->render_ui();
        }
    }
}

void ModuleManager::render_debug()
{
    for (const auto& module : render_modules)
    {
        module->render_debug();
    }
}

void ModuleManager::resize_window(u32 width, u32 height)
{
    for (const auto& module : render_modules)
    {
        module->resize_window(width, height);
    }
}

void ModuleManager::register_module(std::shared_ptr<IModule> module)
{
    registered_modules[module->get_name()] = module;

    if (module->do_start())
    {
        services.push_back(module);
    }
}

void ModuleManager::register_game_module(std::shared_ptr<IModule> module)
{
    registered_modules[module->get_name()] = module;
}

std::shared_ptr<IModule> ModuleManager::get_module(const std::string& name)
{
    auto module_iterator = registered_modules.find(name);
    return module_iterator != registered_modules.end() ? module_iterator->second : nullptr;
}

void ModuleManager::start_modules(const ModulesVector& modules)
{
    for (const auto& module : modules)
    {
        module->do_start();
    }
}

void ModuleManager::stop_modules(const ModulesVector& modules)
{
    for (const auto& module : modules)
    {
        module->do_stop();
    }
}

void ModuleManager::parse_module_config(const std::string& config_file)
{
    update_modules.clear();
    render_modules.clear();

    auto json_file = platform::load_json(config_file);

    json json_update_modules = json_file["update"];
    json json_render_modules = json_file["render"];

    update_modules.reserve(json_update_modules.size());
    for (const auto& json_module : json_update_modules)
    {
        const auto& module_name = json_module.get<std::string>();
        auto        module      = get_module(module_name);
        ASSERT(module.get() != nullptr);
        if (module)
        {
            update_modules.push_back(module);
        }
    }

    render_modules.reserve(json_render_modules.size());
    for (const auto& json_module : json_render_modules)
    {
        const auto& module_name = json_module.get<std::string>();
        auto        module      = get_module(module_name);
        ASSERT(module.get() != nullptr);
        if (module)
        {
            render_modules.push_back(module);
        }
    }
}

} // namespace modules
} // namespace sogas
