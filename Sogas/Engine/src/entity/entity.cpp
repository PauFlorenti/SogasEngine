#include "pch.hpp"

#include <entity/entity.h>
#include <entity/entity_parser.h>
#include <handle/object_manager.h>

namespace sogas
{
DECLARE_OBJECT_MANAGER("entity", Entity);

Entity::~Entity()
{
    // Component i = 0 is not valid.
    for (u32 i = 1; i < HandleManager::get_number_of_defined_types(); ++i)
    {
        if (components[i].is_valid())
        {
            components[i].destroy();
        }
    }
}

void Entity::add_component(Handle::handle_type type, Handle component)
{
    ASSERT(type < Handle::max_types);
    ASSERT(!components[type].is_valid());

    components[type] = component;
    component.set_owner(Handle(this));
}

void Entity::add_component(Handle component)
{
    add_component(component.get_type(), component);
}

const std::string Entity::get_name() const
{
    // TODO Implement name component.
    return {};
}

void Entity::load(const nlohmann::json& json_file, EntityParser& scene)
{
    for (const auto& component : json_file.items())
    {
        auto& component_name = component.key();
        auto& component_data = component.value();

#ifndef NDEBUG
        PINFO("Parsing component %s from json scene %s.",
              component_name.c_str(),
              component_data.dump().c_str());
#endif

        auto object_manager = HandleManager::get_by_name(component_name);
        if (!object_manager)
        {
            PWARN("Warning! Unknown component name %s.", component_name.c_str());
            return;
        }

        const auto component_type   = object_manager->get_type();
        Handle     component_handle = components[component_type];

        if (component_handle.is_valid())
        {
            component_handle.load(component_data, scene);
        }
        else
        {
            component_handle = object_manager->create_handle();
            add_component(component_type, component_handle);
            component_handle.load(component_data, scene);
        }
    }
}

void Entity::on_entity_created()
{
    for (u32 i = 0; i < HandleManager::get_number_of_defined_types(); ++i)
    {
        if (components[i].is_valid())
        {
            components[i].on_entity_created();
        }
    }
}
} // namespace sogas
