#pragma once

namespace pinut
{
namespace resources
{
enum class BufferType
{
    VERTEX,
    INDEX,
    UNIFORM,
    STORAGE,
    STAGING,
    COUNT
};

struct BufferDescriptor
{
    u32        size = 0;
    BufferType type = BufferType::VERTEX;
    void*      data = nullptr;
};
} // namespace resources
} // namespace pinut
