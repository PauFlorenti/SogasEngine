#pragma once

#include <components/base_component.h>
#include <handle/handle.h>
#include <handle/handle_manager.h>
#include <handle/object_manager.h>

namespace pinut::resources
{
  class CommandBuffer;
}
namespace sogas
{
class Entity : public BaseComponent
{
  public:
    Entity() = default;
    ~Entity();

    Handle get(Handle::handle_type component_type) const
    {
        ASSERT(component_type < Handle::max_types);
        return components[component_type];
    }

    template <typename Component>
    Handle get() const
    {
        auto object_manager = get_object_manager<Component>();
        ASSERT(object_manager);
        return components[object_manager->get_type()];
    }

    void render_debug(pinut::resources::CommandBuffer* cmd);

    void add_component(Handle::handle_type type, Handle component);
    void add_component(Handle component);

    const std::string get_name() const;

    void load(const nlohmann::json& json_file, EntityParser& scene);
    void on_entity_created();

  private:
    Handle components[Handle::max_types];
};

template <>
ObjectManager<Entity>* get_object_manager<Entity>();

extern Handle get_entity_by_name(const std::string& name);
} // namespace sogas
