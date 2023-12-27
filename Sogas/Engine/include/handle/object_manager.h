#pragma once

#include <handle/handle_manager.h>

namespace sogas
{
template <typename object_type>
class ObjectManager : public HandleManager
{
  public:
    ObjectManager(const ObjectManager&) = delete;
    ObjectManager(const std::string& name) : objects(nullptr)
    {
        HandleManager::predefined_handle_managers
          [HandleManager::number_predefined_handle_managers++] = this;
        this->name                                             = name;
    }

    void init(u32 max_objects) override
    {
        HandleManager::init(max_objects);

        memory  = new u8[max_objects * sizeof(object_type)];
        objects = static_cast<object_type*>(static_cast<void*>(memory));
    };

    void update_all(f32 delta_time) override
    {
        ASSERT(objects);

        if (number_objects_used <= 0)
        {
            return;
        }

        for (decltype(number_objects_used) i = 0; i < number_objects_used; ++i)
        {
            objects[i].update(delta_time);
        }
    }

    Handle get_handle_from_address(object_type* address)
    {
        auto internal_index = address - objects;
        if (internal_index >= number_objects_used || internal_index < 0)
        {
            return Handle();
        }

        auto  external_index = internal_to_external.at(internal_index);
        auto& external_data  = external_to_internal.at(external_index);
        return Handle(type, external_index, external_data.current_generation);
    }

    object_type* get_address_from_handle(Handle handle)
    {
        if (!handle.get_type() || handle.get_type() != get_type())
        {
            return nullptr;
        }

        ASSERT(handle.get_type() == get_type());

        const auto& external_data = external_to_internal.at(handle.get_external_index());
        if (external_data.current_generation != handle.get_generation())
        {
            return nullptr;
        }

        return objects + external_data.internal_index;
    }

  private:
    void create_object(Handle::handle_index internal_index) override
    {
        object_type* address = objects + internal_index;
        new (address) object_type; // Call constructor into object address.
    }

    void destroy_object(Handle::handle_index internal_index) override
    {
        object_type* address = objects + internal_index;
        address->~object_type();
    }

    void load_object(u32 internal_index, const json& j, EntityParser& context) override
    {
        object_type* address = objects + internal_index;
        address->load(j, context);
    }

    void on_entity_created_object(u32 internal_index) override
    {
        object_type* address = objects + internal_index;
        address->on_entity_created();
    }

    u8*          memory;
    object_type* objects = nullptr;
};

#define DECLARE_OBJECT_MANAGER(object_name, object_class_name)                        \
    ObjectManager<object_class_name> object_manager_##object_class_name(object_name); \
    template <>                                                                       \
    ObjectManager<object_class_name>* get_object_manager<object_class_name>()         \
    {                                                                                 \
        return &object_manager_##object_class_name;                                   \
    }
} // namespace sogas
