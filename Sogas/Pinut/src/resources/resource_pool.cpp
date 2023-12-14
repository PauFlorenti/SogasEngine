#include "pch.hpp"

#include <resources/resource_pool.h>

namespace pinut
{
namespace resources
{
void ResourcePool::init(const u64 new_pool_size, const u64 new_resource_size)
{
    pool_size     = new_pool_size;
    resource_size = new_resource_size;

    const u64 allocation_size = pool_size * (resource_size + sizeof(u32));
    memory                    = (u8*)malloc(allocation_size);

    if (!memory)
    {
        throw std::runtime_error("Not enought memory in resource pool.");
    }

    free_indices      = (u32*)(memory + pool_size * resource_size);
    free_indices_head = 0;

    for (u32 i = 0; i < pool_size; ++i)
    {
        free_indices[i] = i;
    }

    used_indices = 0;
}

void ResourcePool::shutdown()
{
    if (free_indices_head != 0)
    {
        PWARN("Resources not freed.");
        for (u32 i = 0; i < free_indices_head; ++i)
        {
            PWARN("\tResource %u", free_indices[i]);
        }
    }

    ASSERT(used_indices == 0);

    free(memory);
}

u32 ResourcePool::get_resource()
{
    if (free_indices_head < pool_size)
    {
        const u32 free_index = free_indices[free_indices_head++];
        ++used_indices;
        return free_index;
    }

    return INVALID_ID;
}

void ResourcePool::remove_resource(const u32 resource)
{
    free_indices[--free_indices_head] = resource;
    --used_indices;
}

void ResourcePool::remove_all()
{
    free_indices_head = 0;
    used_indices      = 0;

    for (u32 i = 0; i < pool_size; ++i)
    {
        free_indices[i] = i;
    }
}

void* ResourcePool::access_resource(u32 index)
{
    ASSERT(index != INVALID_ID);

    if (index != INVALID_ID)
    {
        return &memory[index * resource_size];
    }

    return nullptr;
}

const void* ResourcePool::access_resource(u32 index) const
{
    ASSERT(index != INVALID_ID);

    if (index != INVALID_ID)
    {
        return &memory[index * resource_size];
    }

    return nullptr;
}
} // namespace resources
} // namespace pinut
