
#include "pch.hpp"

#include <modules/module_manager.h>

namespace sogas
{
namespace modules
{
void ModuleManager::boot()
{
}

void ModuleManager::clear()
{
}

void ModuleManager::update(f32 /*delta_time*/)
{
}

void ModuleManager::render()
{
}

void ModuleManager::render_ui()
{
}

void ModuleManager::render_debug()
{
}

void ModuleManager::register_module(std::shared_ptr<IModule> /*module*/)
{
}

void ModuleManager::register_game_module(std::shared_ptr<IModule> module)
{
}
} // namespace modules
} // namespace sogas