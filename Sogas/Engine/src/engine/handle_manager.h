#pragma once

#include <engine/handle.h>

namespace sogas::engine
{
class HandleManager final
{
    static constexpr u32 max_total_objects = Handle::max_types;
    static bool          handle_pending_destroy{false};

    struct ExternalData
    {
        Handle::handle_index      internal_index;
        Handle::handle_index      nex_external_index;
        Handle                    current_owner;
        Handle::handle_generation current_generation : Handle::num_bits_generation;

        ExternalData()
        : internal_index(0),
          nex_external_index(0),
          current_generation(0)
        {
        }
    };

  public:
    HandleManager(const HandleManager&) = delete;
    ~HandleManager()                    = default;

  protected:
    Handle::handle_type type;

    std::vector<Handle::handle_index> external_to_internal;
    std::vector<ExternalData> internal_to_external;

    u32 number_objects_used;
    u32 next_free_handle_external_index;
    u32 last_free_handle_external_index;

    std::vector<Handle> pending_destroy;
};
} // namespace sogas::engine