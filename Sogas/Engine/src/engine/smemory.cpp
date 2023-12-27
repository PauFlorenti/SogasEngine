#include "pch.hpp"

#include <engine/smemory.h>

namespace sogas
{
static u64 memory_align(const u64 size, const u64 alignment)
{
    const u64 alignment_mask = alignment - 1;
    return (size + alignment_mask) & ~alignment_mask;
}

void StackAllocator::init(const u64 size)
{
    memory           = (u8*)malloc(size);
    total_memory     = size;
    allocated_memory = 0;
}

void StackAllocator::shutdown()
{
    free(memory);
}

void* StackAllocator::allocate(const u64 size, const u64 alignment)
{
    ASSERT(size > 0);

    const u64 new_memory_start = memory_align(allocated_memory, alignment);

    ASSERT(new_memory_start < total_memory);

    const u64 new_allocated_memory = new_memory_start + size;

    if (new_allocated_memory > total_memory)
    {
        PWARN("Not enough memory to allocate.");
        return nullptr;
    }

    allocated_memory = new_allocated_memory;
    return memory + new_memory_start;
}

void StackAllocator::deallocate(void* pointer)
{
    ASSERT(pointer >= memory);
    ASSERT(pointer < memory + total_memory);
    ASSERT(pointer < memory + allocated_memory);

    const u64 memory_at_pointer = (u8*)pointer - memory;
    allocated_memory            = memory_at_pointer;
}

u64 StackAllocator::get_marker()
{
    return allocated_memory;
}

void StackAllocator::free_marker(u64 marker)
{
    if (allocated_memory - marker > 0)
    {
        allocated_memory = marker;
    }
}

void StackAllocator::clear()
{
    allocated_memory = 0;
}
} // namespace sogas
