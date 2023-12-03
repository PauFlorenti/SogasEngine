#pragma once

namespace pinut
{
namespace resources
{
struct ResourcePool
{
    void init(const u64 pool_size, const u64 resource_size);
    void shutdown();

    u32  get_resource();
    void remove_resource(const u32 resource);
    void remove_all();

    void*       access_resource(u32 index);
    const void* access_resource(u32 index) const;

    u8*  memory       = nullptr;
    u32* free_indices = nullptr;

    u32 free_indices_head = 0;
    u64 pool_size         = 0;
    u64 resource_size     = 0;
    u32 used_indices      = 0;
};
} // namespace resources
} // namespace pinut
