#pragma once

namespace pinut
{
namespace resources
{

using ResourceHandle = u32;

struct BufferHandle
{
    ResourceHandle id;
};

struct TextureHandle
{
    ResourceHandle id;
};

struct ShaderStateHandle
{
    ResourceHandle id;
};
} // namespace resources
} // namespace pinut
