#pragma once

#include <handle/handle.h>

namespace sogas
{
namespace engine
{
namespace handle
{
using HandleManagerArray = std::array<HandleManager*, Handle::max_types>;

class HandleManager
{
    static constexpr u32 max_total_objects =
      1 << Handle::num_bits_index; // The maximum number of objects per type
    static bool handle_pending_destroy; // Mark as true when objects are pending to destroy.

    struct ExternalData
    {
        Handle::handle_index      internal_index; // Holds the position of the object in the array.
        Handle::handle_index      next_external_index;
        Handle                    current_owner;
        Handle::handle_generation current_generation : Handle::num_bits_generation;

        ExternalData() : internal_index(0), next_external_index(0), current_generation(0)
        {
        }
    };

  public:
    HandleManager()
    : type(0),
      number_objects_used(0),
      next_free_handle_external_index(0),
      last_free_handle_external_index(0)
    {
    }
    HandleManager(const HandleManager&) = delete;
    ~HandleManager()                    = default;

    virtual void init(const u32 max_objects);
    bool         is_valid(Handle h) const;

    const std::string get_name() const
    {
        return name;
    }
    const Handle::handle_type get_type() const
    {
        return type;
    }
    const u32 get_size() const
    {
        return number_objects_used;
    }
    const u32 get_capacity() const
    {
        return max_total_objects;
    }

    //bool destroy_pending_objects();

    Handle create_handle();
    void   destroy_handle(Handle h);

    void   set_owner(Handle item, Handle owner);
    Handle get_owner(Handle item);

    static HandleManager* get_by_type(Handle::handle_type type);
    static HandleManager* get_by_name(const std::string& name);
    static const u32      get_number_of_defined_types();
    static void           destroy_all_pending_objects();

    virtual void update_all(f32 delta_time) = 0;

    static HandleManagerArray predefined_handle_managers;
    static u32                number_predefined_handle_managers;

  protected:
    // Shared by all managers
    static u32                                   next_type_of_handle_manager;
    static HandleManagerArray                    all_handle_managers;
    static std::map<std::string, HandleManager*> all_handle_managers_by_name;

    Handle::handle_type type; // The type of object this handle manager manages.

    std::vector<ExternalData>         external_to_internal;
    std::vector<Handle::handle_index> internal_to_external;

    u32 number_objects_used;
    u32 next_free_handle_external_index;
    u32 last_free_handle_external_index;

    HandleVector pending_destroy; // To be destroyed in a safe moment.

    std::string name{};

    virtual void create_object(Handle::handle_index index)  = 0;
    virtual void destroy_object(Handle::handle_index index) = 0;

    // TODO Add vector holding all pending deleted handles.
};

} // namespace handle
} // namespace engine
} // namespace sogas
