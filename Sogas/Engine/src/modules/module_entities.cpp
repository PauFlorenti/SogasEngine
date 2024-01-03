#include "pch.hpp"

#include <handle/handle_manager.h>
#include <modules/module_entities.h>

namespace
{
void load_managers(const json& j, std::vector<sogas::HandleManager*>& managers)
{
    managers.clear();
    std::vector<std::string> names = j;

    for (auto& name : names)
    {
        auto object_manager = sogas::HandleManager::get_by_name(name.c_str());
        ASSERT(object_manager);
        managers.push_back(object_manager);
    }
}
} // namespace
namespace sogas
{
namespace modules
{
bool EntityModule::start()
{
    PINFO("Entity module starting!");

    auto j = platform::load_json("../../Sogas/Engine/data/components.json");

    std::map<std::string, i32> sizes        = j["sizes"];
    i32                        default_size = sizes["default"];

    std::map<std::string, i32> initialization_order = j["init_order"];

    std::sort(HandleManager::predefined_handle_managers.data(),
              HandleManager::predefined_handle_managers.data() +
                HandleManager::number_predefined_handle_managers,
              [&initialization_order](HandleManager* hm1, HandleManager* hm2)
              {
                  i32 priority1 = initialization_order[hm1->get_name()];
                  i32 priority2 = initialization_order[hm2->get_name()];
                  return priority1 > priority2;
              });

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
        auto component_iterator = sizes.find(handle_manager->get_name());
        auto component_size =
          (component_iterator == sizes.end()) ? default_size : component_iterator->second;
        handle_manager->init(component_size);
        ++index;
    }

    // TODO: Load component managers ...
    load_managers(j["update"], managers_to_update);
    load_managers(j["render_debug"], managers_to_render_debug);

    return true;
}

void EntityModule::stop()
{
    PINFO("Entity module stoping!");
}

void EntityModule::update(f32 delta_time)
{
    for (auto object_manager : managers_to_update)
    {
        object_manager->update_all(delta_time);
    }

    // TODO Destroy all pending objects.
}

void EntityModule::render_debug(pinut::resources::CommandBuffer* cmd)
{
    //TODO get render debug pipeline
    for (auto object_manager : managers_to_render_debug)
    {
        object_manager->render_debug_all(cmd);
    }
}
} // namespace modules
} // namespace sogas
