#include "pch.hpp"

#include <handle/handle_manager.h>

namespace sogas
{
namespace engine
{
namespace handle
{
u32                                   HandleManager::next_type_of_handle_manager = 1;
HandleManagerArray                    HandleManager::all_handle_managers;
std::map<std::string, HandleManager*> HandleManager::all_handle_managers_by_name;

HandleManagerArray HandleManager::predefined_handle_managers;
u32                HandleManager::number_predefined_handle_managers = 0;

bool HandleManager::handle_pending_destroy = false;

void HandleManager::init(const u32 max_objects)
{
    ASSERT(max_objects < max_total_objects);
    ASSERT(max_objects > 0);

    // When init, all managers should have types as 0.
    if (type != 0)
    {
        ERROR("HandleManager with type != 0.");
        return;
    }

    type = next_type_of_handle_manager++;

    all_handle_managers[type]               = this;
    all_handle_managers_by_name[get_name()] = this;

    u32 i{0};
    for (auto& index : external_to_internal)
    {
        index.current_generation = 1;
        index.internal_index     = INVALID_ID;

        if (i != max_objects - 1)
        {
            index.next_external_index = i + 1;
        }
        else
        {
            index.next_external_index = INVALID_ID;
        }

        internal_to_external[i] = INVALID_ID;
        ++i;
    }

    next_free_handle_external_index = 0;
    last_free_handle_external_index = max_objects - 1;
}

bool HandleManager::is_valid(Handle h) const
{
    ASSERT(h.get_type() == type);
    ASSERT(h.get_external_index() < number_objects_used);

    auto& external_data = external_to_internal.at(h.get_external_index());
    return external_data.current_generation == h.get_generation();
}

Handle HandleManager::create_handle()
{
    ASSERT(type != 0);

    ASSERT(next_free_handle_external_index != INVALID_ID);
    ASSERT(number_objects_used < get_capacity());

    Handle::handle_index external_index = next_free_handle_external_index;
    auto&                external_data  = external_to_internal.at(external_index);
    external_data.internal_index        = number_objects_used;

    ASSERT(external_data.current_owner == Handle());

    internal_to_external[external_data.internal_index] = external_index;

    create_object(external_data.internal_index);

    ++number_objects_used;
    next_free_handle_external_index = external_data.next_external_index;
    ASSERT(next_free_handle_external_index != INVALID_ID);

    // Not necessary while object is alive. This is only used to point to the next
    // index for when needed creation.
    external_data.next_external_index = INVALID_ID;

    return Handle(type, external_index, external_data.current_generation);
}

void HandleManager::destroy_handle(Handle h)
{
    if (!h.is_valid())
    {
        return;
    }

    handle_pending_destroy = true;

    //objects_to_destroy.push_back(h);

    auto& external_data = external_to_internal.at(h.get_external_index());
    external_data.current_generation++;
}

void HandleManager::set_owner(Handle item, Handle owner)
{
    ASSERT(item.get_type() == type);
    ASSERT(item.is_valid());
    ASSERT(owner.is_valid());

    auto& external_data         = external_to_internal.at(item.get_external_index());
    external_data.current_owner = owner;
}

Handle HandleManager::get_owner(Handle item)
{
    ASSERT(item.get_type() == type);

    if (!item.is_valid())
    {
        return Handle();
    }

    auto& external_data = external_to_internal.at(item.get_external_index());
    return external_data.current_owner;
}

HandleManager* HandleManager::get_by_type(Handle::handle_type type)
{
    return all_handle_managers.at(type);
}

HandleManager* HandleManager::get_by_name(const std::string& name)
{
    auto it = all_handle_managers_by_name.find(name);
    if (it != all_handle_managers_by_name.end())
    {
        return it->second;
    }
    return nullptr;
}
const u32 HandleManager::get_number_of_defined_types()
{
    return next_type_of_handle_manager;
}

void HandleManager::destroy_all_pending_objects()
{
    if (!handle_pending_destroy)
    {
        return;
    }

    // TODO delete all pending objects.
}

} // namespace handle
} // namespace engine
} // namespace sogas
