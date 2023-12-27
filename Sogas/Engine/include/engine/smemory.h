#pragma once

namespace sogas
{
#define kb(size) (size * 1024)
#define mb(size) (size * 1024 * 1024)
#define gb(size) (size * 1024 * 1024)

struct Allocator
{
    virtual void* allocate(const u64 size, const u64 alignment) = 0;
    virtual void  deallocate(void* pointer)                     = 0;
};

struct StackAllocator : public Allocator
{
    void init(const u64 size);
    void shutdown();

    void* allocate(const u64 size, const u64 alignment) override;
    void  deallocate(void* pointer) override;

    u64  get_marker();
    void free_marker(u64 marker);

    void clear();

    u8* memory           = nullptr;
    u64 total_memory     = 0;
    u64 allocated_memory = 0;
};
} // namespace sogas
