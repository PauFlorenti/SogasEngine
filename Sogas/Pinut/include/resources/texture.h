#pragma once

namespace pinut
{
namespace resources
{
struct TextureDescriptor
{
    u32         width;
    u32         height;
    const char* name          = nullptr;
    u8          channel_count = 0;
    u32         mip_levels    = 1;
    void*       data;
};

class Texture
{
};
} // namespace resources
} // namespace pinut
