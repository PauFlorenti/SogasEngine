
#include "pch.hpp"

#include <components/basic/name_component.h>
#include <handle/object_manager.h>

namespace sogas
{
DECLARE_OBJECT_MANAGER("name", NameComponent);

std::unordered_map<std::string, Handle> NameComponent::all_names;

const std::string NameComponent::get_name() const
{
    return name;
}

void NameComponent::set_name(const std::string& new_name)
{
    ASSERT(new_name.size() < max_length);

    auto it = all_names.find(new_name);

    if (it == all_names.end())
    {
        strcpy_s(name, new_name.c_str());
        //all_names[name] = Handle(this);
    }
}

void NameComponent::load(const json& j, EntityParser& /*context*/)
{
    ASSERT(j.is_string());
    set_name(j.get<std::string>().c_str());
}
} // namespace sogas
