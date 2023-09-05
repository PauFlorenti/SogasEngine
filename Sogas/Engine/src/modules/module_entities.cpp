#include "pch.hpp"

#include <modules/module_entities.h>

namespace sogas
{
namespace modules
{
using namespace engine::handle;
bool EntityModule::start()
{
    INFO("Entity module starting!");

    ASSERT(HandleManager::predefined_handle_managers.at(0) != nullptr);
    ASSERT(strcmp(HandleManager::predefined_handle_managers.at(0)->get_name().c_str(), "entity") ==
           0);

    u32 index{0};
    for (const auto& handle_manager : HandleManager::predefined_handle_managers)
    {
        // Initialize only the predefined handle managers.
        if (index >= HandleManager::number_predefined_handle_managers)
        {
            break;
        }

        // TODO: Read from components.json
        handle_manager->init(1024);
        ++index;
    }

    // TODO: Load component managers ...

    return true;
}

void EntityModule::stop()
{
    INFO("Entity module stoping!");
}

void EntityModule::update(f32 /*delta_time*/)
{
}
} // namespace modules
} // namespace sogas
