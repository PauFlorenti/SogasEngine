#pragma once

#include <components/base_component.h>
#include <handle/handle.h>

namespace sogas
{
namespace engine
{
namespace components
{
using namespace handle;
class Entity : public BaseComponent
{
  public:
    Entity() = default;
    ~Entity(){}; // TODO Set all components attach to the entity for pending destroy.

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

    void add_component(Handle::handle_type type, Handle component);
    void add_component(Handle component);

    const std::string get_name() const;

    void load(const nlohmann::json& json_file, Scene& scene);

  private:
    Handle components[Handle::max_types];
};
} // namespace components
} // namespace engine
} // namespace sogas
