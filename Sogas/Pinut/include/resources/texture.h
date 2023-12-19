#pragma once

namespace pinut
{
namespace resources
{
enum class TextureType
{
    TEXTURE_1D,
    TEXTURE_2D,
    TEXTURE_3D,
    COUNT
};

enum class TextureViewType
{
    TEXTURE_1D,
    TEXTURE_2D,
    TEXTURE_3D,
    TEXTURE_1D_ARRAY,
    TEXTURE_2D_ARRAY,
    TEXTURE_CUBE,
    TEXTURE_CUBE_ARRAY,
    COUNT
};

enum class TextureFormat
{
    UNDEFINED,

    R8_UNORM,
    R8_SNORM,
    R8_USCALED,
    R8_SSCALED,
    R8_UINT,
    R8_SINT,
    R8_SRGB,
    R8G8_UNORM,
    R8G8_SNORM,
    R8G8_USCALED,
    R8G8_SSCALED,
    R8G8_UINT,
    R8G8_SINT,
    R8G8_SRGB,
    R8G8B8_UNORM,
    R8G8B8_SNORM,
    R8G8B8_USCALED,
    R8G8B8_SSCALED,
    R8G8B8_UINT,
    R8G8B8_SINT,
    R8G8B8_SRGB,
    R8G8B8A8_UNORM,
    R8G8B8A8_SNORM,
    R8G8B8A8_USCALED,
    R8G8B8A8_SSCALED,
    R8G8B8A8_UINT,
    R8G8B8A8_SINT,
    R8G8B8A8_SRGB,

    R16_UNORM,
    R16_SNORM,
    R16_USCALED,
    R16_SSCALED,
    R16_UINT,
    R16_SINT,
    R16_SFLOAT,
    R16G16_UNORM,
    R16G16_SNORM,
    R16G16_USCALED,
    R16G16_SSCALED,
    R16G16_UINT,
    R16G16_SINT,
    R16G16_SFLOAT,
    R16G16B16_UNORM,
    R16G16B16_SNORM,
    R16G16B16_USCALED,
    R16G16B16_SSCALED,
    R16G16B16_UINT,
    R16G16B16_SINT,
    R16G16B16_SFLOAT,
    R16G16B16A16_UNORM,
    R16G16B16A16_SNORM,
    R16G16B16A16_USCALED,
    R16G16B16A16_SSCALED,
    R16G16B16A16_UINT,
    R16G16B16A16_SINT,
    R16G16B16A16_SFLOAT,

    R32_UINT,
    R32_SINT,
    R32_SFLOAT,
    R32G32_UINT,
    R32G32_SINT,
    R32G32_SFLOAT,
    R32G32B32_UINT,
    R32G32B32_SINT,
    R32G32B32_SFLOAT,
    R32G32B32A32_UINT,
    R32G32B32A32_SINT,
    R32G32B32A32_SFLOAT,

    R64_UINT,
    R64_SINT,
    R64_SFLOAT,
    R64G64_UINT,
    R64G64_SINT,
    R64G64_SFLOAT,
    R64G64B64_UINT,
    R64G64B64_SINT,
    R64G64B64_SFLOAT,
    R64G64B64A64_UINT,
    R64G64B64A64_SINT,
    R64G64B64A64_SFLOAT,

    D32_SFLOAT,
    D16_UNORM_S8_UINT,
    D24_UNORM_S8_UINT,
    D32_SFLOAT_S8_UINT,

    COUNT
};

struct TextureDescriptor
{
    void*         data          = nullptr;
    u16           width         = 1;
    u16           height        = 1;
    u16           depth         = 1;
    TextureType   type          = TextureType::TEXTURE_2D;
    TextureFormat format        = TextureFormat::R8G8B8A8_SRGB;
    u8            channel_count = 4;
    u8            mip_levels    = 1;
    const char*   name          = nullptr;

    TextureDescriptor& set_size(const u16 texture_width,
                                const u16 texture_height,
                                const u16 texture_depth = 1)
    {
        this->width  = texture_width;
        this->height = texture_height;
        this->depth  = texture_depth;
        return *this;
    }

    TextureDescriptor& set_type(TextureType texture_type)
    {
        this->type = texture_type;
        return *this;
    }

    TextureDescriptor& set_format(TextureFormat texture_format)
    {
        this->format = texture_format;
        return *this;
    }

    TextureDescriptor& set_flags(u8 mipmaps)
    {
        this->mip_levels = mipmaps;
        return *this;
    }

    TextureDescriptor& add_name(const char* texture_name)
    {
        this->name = texture_name;
    }

    TextureDescriptor& set_data(void* initial_data)
    {
        ASSERT(data != nullptr);

        this->data = initial_data;
        return *this;
    }
};
} // namespace resources
} // namespace pinut
