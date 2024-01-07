
#include "pch.hpp"

#include <handle/handle.h>
#include <handle/handle_manager.h>

namespace sogas
{
const std::string Handle::get_type_name() const
{
    if (auto handle_manager = HandleManager::get_by_type(type))
    {
        return handle_manager->get_name();
    }

    return "<invalid>";
}

bool Handle::is_valid() const
{
    auto handle_manager = HandleManager::get_by_type(type);
    return handle_manager && handle_manager->is_valid(*this);
}

void Handle::destroy()
{
    if (auto handle_manager = HandleManager::get_by_type(type))
    {
        handle_manager->destroy_handle(*this);
    }
}

void Handle::render_debug(pinut::resources::CommandBuffer* cmd)
{
    if (auto handle_manager = HandleManager::get_by_type(type))
    {
        handle_manager->render_debug(*this, cmd);
    }
}

void Handle::render_debug_menu()
{
    if (auto handle_manager = HandleManager::get_by_type(type))
    {
        handle_manager->render_debug_menu(*this);
    }
}

void Handle::load(const nlohmann::json& j, EntityParser& context)
{
    if (auto handle_manager = HandleManager::get_by_type(type))
    {
        handle_manager->load(*this, j, context);
    }
}

void Handle::on_entity_created()
{
    if (auto handle_manager = HandleManager::get_by_type(type))
    {
        handle_manager->on_entity_created(*this);
    }
}

void Handle::set_owner(Handle h)
{
    if (auto handle_manager = HandleManager::get_by_type(type))
    {
        handle_manager->set_owner(*this, h);
    }
}

Handle Handle::get_owner() const
{
    if (auto handle_manager = HandleManager::get_by_type(type))
    {
        return handle_manager->get_owner(*this);
    }

    return Handle();
}
} // namespace sogas
