#include "pch.hpp"

#include <vulkan/utils/vulkan_initializers.h>
#include <vulkan/utils/vulkan_utils.h>

namespace pinut
{
namespace vulkan
{
using namespace resources;
VkCullModeFlags get_cull_mode_flag(CullMode mode)
{
    switch (mode)
    {
        default:
        case CullMode::NONE:
            return VK_CULL_MODE_NONE;
            break;
        case CullMode::FRONT:
            return VK_CULL_MODE_FRONT_BIT;
            break;
        case CullMode::BACK:
            return VK_CULL_MODE_BACK_BIT;
            break;
        case CullMode::FRONT_AND_BACK:
            return VK_CULL_MODE_FRONT_AND_BACK;
            break;
    }
}

VkFrontFace get_front_face_flag(FrontFace face)
{
    switch (face)
    {
        default:
        case pinut::resources::COUNTER_CLOCKWISE:
            return VK_FRONT_FACE_COUNTER_CLOCKWISE;
            break;
        case pinut::resources::CLOCKWISE:
            return VK_FRONT_FACE_COUNTER_CLOCKWISE;
            break;
    }
}

VkPolygonMode get_cull_mode_flag(FillMode mode)
{
    switch (mode)
    {
        default:
        case pinut::resources::FILL:
            return VK_POLYGON_MODE_FILL;
            break;
        case pinut::resources::LINE:
            return VK_POLYGON_MODE_LINE;
            break;
        case pinut::resources::POINT:
            return VK_POLYGON_MODE_POINT;
            break;
    }
}
VkFormat get_vertex_input_format(resources::VertexInputFormatType format)
{
    switch (format)
    {
        default:
        case pinut::resources::VertexInputFormatType::COUNT:
            PWARN("Invalid format type provided. Float set as default.");
            return VK_FORMAT_MAX_ENUM;
            break;
        case pinut::resources::VertexInputFormatType::FLOAT:
            return VK_FORMAT_R32_SFLOAT;
            break;
        case pinut::resources::VertexInputFormatType::INT:
            return VK_FORMAT_R32_SINT;
            break;
        case pinut::resources::VertexInputFormatType::UINT:
            return VK_FORMAT_R32_UINT;
            break;
        case pinut::resources::VertexInputFormatType::BOOL:
            return VK_FORMAT_R8_UNORM;
            break;
        case pinut::resources::VertexInputFormatType::VEC2:
            return VK_FORMAT_R32G32_SFLOAT;
            break;
        case pinut::resources::VertexInputFormatType::VEC3:
            return VK_FORMAT_R32G32B32_SFLOAT;
            break;
        case pinut::resources::VertexInputFormatType::VEC4:
            return VK_FORMAT_R32G32B32A32_SFLOAT;
            break;
    }
}
VkVertexInputRate get_vertex_input_rate(resources::VertexInputRate input_rate)
{
    switch (input_rate)
    {
        default:
        case pinut::resources::VertexInputRate::PER_VERTEX:
            return VK_VERTEX_INPUT_RATE_VERTEX;
            break;
        case pinut::resources::VertexInputRate::PER_INSTANCE:
            return VK_VERTEX_INPUT_RATE_INSTANCE;
            break;
        case pinut::resources::VertexInputRate::COUNT:
            PWARN("Invalid input rate provided.");
            return VK_VERTEX_INPUT_RATE_MAX_ENUM;
            break;
    }
}
VkPrimitiveTopology get_topology(resources::TopologyType topology)
{
    switch (topology)
    {
        case pinut::resources::TopologyType::POINT:
            return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
            break;
        case pinut::resources::TopologyType::LINE:
            return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
            break;
        case pinut::resources::TopologyType::LINE_STRIP:
            return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
            break;
        default:
        case pinut::resources::TopologyType::TRIANGLE:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            break;
        case pinut::resources::TopologyType::TRIANGLE_STRIP:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
            break;
        case pinut::resources::TopologyType::TRIANGLE_FAN:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
            break;
        case pinut::resources::TopologyType::COUNT:
            PWARN("Invalid topology type provided.");
            return VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
            break;
    }
}
VkBufferUsageFlags get_buffer_usage_flag(resources::BufferType buffer_type)
{
    switch (buffer_type)
    {
        case pinut::resources::BufferType::VERTEX:
            return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            break;
        case pinut::resources::BufferType::INDEX:
            return VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
            break;
        default:
        case pinut::resources::BufferType::UNIFORM:
            return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            break;
        case pinut::resources::BufferType::STORAGE:
            return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
            break;
        case pinut::resources::BufferType::COUNT:
            return VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
            break;
    }
}
VkDescriptorType get_descriptor_type(resources::DescriptorType descriptor_type)
{
    switch (descriptor_type)
    {
        case pinut::resources::DescriptorType::SAMPLER:
            return VK_DESCRIPTOR_TYPE_SAMPLER;
            break;
        case pinut::resources::DescriptorType::COMBINED_IMAGE_SAMPLER:
            return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            break;
        case pinut::resources::DescriptorType::SAMPLED_IMAGE:
            return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            break;
        case pinut::resources::DescriptorType::STORAGE_IMAGE:
            return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
            break;
        default:
        case pinut::resources::DescriptorType::UNIFORM:
            return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            break;
        case pinut::resources::DescriptorType::STORAGE:
            return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            break;
        case pinut::resources::DescriptorType::UNIFORM_DYNAMIC:
            return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
            break;
        case pinut::resources::DescriptorType::STORAGE_DYNAMIC:
            return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
            break;
        case pinut::resources::DescriptorType::INPUT_ATTACHMENT:
            return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
            break;
        case pinut::resources::DescriptorType::COUNT:
            return VK_DESCRIPTOR_TYPE_MAX_ENUM;
            break;
    }
}
VkShaderStageFlags get_shader_stage_flag(resources::ShaderStageType shader_stage)
{
    switch (shader_stage)
    {
        case pinut::resources::ShaderStageType::VERTEX:
            return VK_SHADER_STAGE_VERTEX_BIT;
            break;
        case pinut::resources::ShaderStageType::GEOMETRY:
            return VK_SHADER_STAGE_GEOMETRY_BIT;
            break;
        case pinut::resources::ShaderStageType::FRAGMENT:
            return VK_SHADER_STAGE_FRAGMENT_BIT;
            break;
        case pinut::resources::ShaderStageType::COMPUTE:
            return VK_SHADER_STAGE_COMPUTE_BIT;
            break;
        default:
        case pinut::resources::ShaderStageType::MAX_SHADER_TYPE:
            PWARN("Undefined shader stage provided. Binding to all shader stages.");
            return VK_SHADER_STAGE_ALL;
            break;
    }
}

VkIndexType get_buffer_index_type(resources::BufferIndexType index_type)
{
    switch (index_type)
    {
        default:
        case resources::BufferIndexType::UINT16:
            return VK_INDEX_TYPE_UINT16;
            break;
        case resources::BufferIndexType::UINT32:
            return VK_INDEX_TYPE_UINT32;
            break;
        case resources::BufferIndexType::COUNT:
            PWARN("Invalid buffer index type provided.");
            return VK_INDEX_TYPE_MAX_ENUM;
            break;
    }
}
VkFormat get_texture_format(resources::TextureFormat format)
{
    switch (format)
    {
        case pinut::resources::TextureFormat::UNDEFINED:
            return VK_FORMAT_UNDEFINED;
            break;
        case pinut::resources::TextureFormat::R8_UNORM:
            return VK_FORMAT_R8_UNORM;
            break;
        case pinut::resources::TextureFormat::R8_SNORM:
            return VK_FORMAT_R8_SNORM;
            break;
        case pinut::resources::TextureFormat::R8_USCALED:
            return VK_FORMAT_R8_USCALED;
            break;
        case pinut::resources::TextureFormat::R8_SSCALED:
            return VK_FORMAT_R8_SSCALED;
            break;
        case pinut::resources::TextureFormat::R8_UINT:
            return VK_FORMAT_R8_UINT;
            break;
        case pinut::resources::TextureFormat::R8_SINT:
            return VK_FORMAT_R8_SINT;
            break;
        case pinut::resources::TextureFormat::R8_SRGB:
            return VK_FORMAT_R8_SRGB;
            break;
        case pinut::resources::TextureFormat::R8G8_UNORM:
            return VK_FORMAT_R8G8_UNORM;
            break;
        case pinut::resources::TextureFormat::R8G8_SNORM:
            return VK_FORMAT_R8G8_SNORM;
            break;
        case pinut::resources::TextureFormat::R8G8_USCALED:
            return VK_FORMAT_R8G8_USCALED;
            break;
        case pinut::resources::TextureFormat::R8G8_SSCALED:
            return VK_FORMAT_R8G8_SSCALED;
            break;
        case pinut::resources::TextureFormat::R8G8_UINT:
            return VK_FORMAT_R8G8_UINT;
            break;
        case pinut::resources::TextureFormat::R8G8_SINT:
            return VK_FORMAT_R8G8_SINT;
            break;
        case pinut::resources::TextureFormat::R8G8_SRGB:
            return VK_FORMAT_R8G8_SRGB;
            break;
        case pinut::resources::TextureFormat::R8G8B8_UNORM:
            return VK_FORMAT_R8G8B8_UNORM;
            break;
        case pinut::resources::TextureFormat::R8G8B8_SNORM:
            return VK_FORMAT_R8G8B8_SNORM;
            break;
        case pinut::resources::TextureFormat::R8G8B8_USCALED:
            return VK_FORMAT_R8G8B8_USCALED;
            break;
        case pinut::resources::TextureFormat::R8G8B8_SSCALED:
            return VK_FORMAT_R8G8B8_SSCALED;
            break;
        case pinut::resources::TextureFormat::R8G8B8_UINT:
            return VK_FORMAT_R8G8B8_UINT;
            break;
        case pinut::resources::TextureFormat::R8G8B8_SINT:
            return VK_FORMAT_R8G8B8_SINT;
            break;
        case pinut::resources::TextureFormat::R8G8B8_SRGB:
            return VK_FORMAT_R8G8B8_SRGB;
            break;
        case pinut::resources::TextureFormat::R8G8B8A8_UNORM:
            return VK_FORMAT_R8G8B8A8_UNORM;
            break;
        case pinut::resources::TextureFormat::R8G8B8A8_SNORM:
            return VK_FORMAT_R8G8B8A8_SNORM;
            break;
        case pinut::resources::TextureFormat::R8G8B8A8_USCALED:
            return VK_FORMAT_R8G8B8A8_USCALED;
            break;
        case pinut::resources::TextureFormat::R8G8B8A8_SSCALED:
            return VK_FORMAT_R8G8B8A8_SSCALED;
            break;
        case pinut::resources::TextureFormat::R8G8B8A8_UINT:
            return VK_FORMAT_R8G8B8A8_UINT;
            break;
        case pinut::resources::TextureFormat::R8G8B8A8_SINT:
            return VK_FORMAT_R8G8B8A8_SINT;
            break;
        case pinut::resources::TextureFormat::R8G8B8A8_SRGB:
            return VK_FORMAT_R8G8B8A8_SRGB;
            break;
        case pinut::resources::TextureFormat::R16_UNORM:
            return VK_FORMAT_R16_UNORM;
            break;
        case pinut::resources::TextureFormat::R16_SNORM:
            return VK_FORMAT_R16_SNORM;
            break;
        case pinut::resources::TextureFormat::R16_USCALED:
            return VK_FORMAT_R16_USCALED;
            break;
        case pinut::resources::TextureFormat::R16_SSCALED:
            return VK_FORMAT_R16_SSCALED;
            break;
        case pinut::resources::TextureFormat::R16_UINT:
            return VK_FORMAT_R16_UINT;
            break;
        case pinut::resources::TextureFormat::R16_SINT:
            return VK_FORMAT_R16_SINT;
            break;
        case pinut::resources::TextureFormat::R16_SFLOAT:
            return VK_FORMAT_R16_SFLOAT;
            break;
        case pinut::resources::TextureFormat::R16G16_UNORM:
            return VK_FORMAT_R16G16_UNORM;
            break;
        case pinut::resources::TextureFormat::R16G16_SNORM:
            return VK_FORMAT_R16G16_SNORM;
            break;
        case pinut::resources::TextureFormat::R16G16_USCALED:
            return VK_FORMAT_R16G16_USCALED;
            break;
        case pinut::resources::TextureFormat::R16G16_SSCALED:
            return VK_FORMAT_R16G16_SSCALED;
            break;
        case pinut::resources::TextureFormat::R16G16_UINT:
            return VK_FORMAT_R16G16_UINT;
            break;
        case pinut::resources::TextureFormat::R16G16_SINT:
            return VK_FORMAT_R16G16_SINT;
            break;
        case pinut::resources::TextureFormat::R16G16_SFLOAT:
            return VK_FORMAT_R16G16_SFLOAT;
            break;
        case pinut::resources::TextureFormat::R16G16B16_UNORM:
            return VK_FORMAT_R16G16B16_UNORM;
            break;
        case pinut::resources::TextureFormat::R16G16B16_SNORM:
            return VK_FORMAT_R16G16B16_SNORM;
            break;
        case pinut::resources::TextureFormat::R16G16B16_USCALED:
            return VK_FORMAT_R16G16B16_USCALED;
            break;
        case pinut::resources::TextureFormat::R16G16B16_SSCALED:
            return VK_FORMAT_R16G16B16_SSCALED;
            break;
        case pinut::resources::TextureFormat::R16G16B16_UINT:
            return VK_FORMAT_R16G16B16_UINT;
            break;
        case pinut::resources::TextureFormat::R16G16B16_SINT:
            return VK_FORMAT_R16G16B16_SINT;
            break;
        case pinut::resources::TextureFormat::R16G16B16_SFLOAT:
            return VK_FORMAT_R16G16B16_SFLOAT;
            break;
        case pinut::resources::TextureFormat::R16G16B16A16_UNORM:
            return VK_FORMAT_R16G16B16A16_UNORM;
            break;
        case pinut::resources::TextureFormat::R16G16B16A16_SNORM:
            return VK_FORMAT_R16G16B16A16_SNORM;
            break;
        case pinut::resources::TextureFormat::R16G16B16A16_USCALED:
            return VK_FORMAT_R16G16B16A16_USCALED;
            break;
        case pinut::resources::TextureFormat::R16G16B16A16_SSCALED:
            return VK_FORMAT_R16G16B16A16_SSCALED;
            break;
        case pinut::resources::TextureFormat::R16G16B16A16_UINT:
            return VK_FORMAT_R16G16B16A16_UINT;
            break;
        case pinut::resources::TextureFormat::R16G16B16A16_SINT:
            return VK_FORMAT_R16G16B16A16_SINT;
            break;
        case pinut::resources::TextureFormat::R16G16B16A16_SFLOAT:
            return VK_FORMAT_R16G16B16A16_SFLOAT;
            break;
        case pinut::resources::TextureFormat::R32_UINT:
            return VK_FORMAT_R32_UINT;
            break;
        case pinut::resources::TextureFormat::R32_SINT:
            return VK_FORMAT_R32_SINT;
            break;
        case pinut::resources::TextureFormat::R32_SFLOAT:
            return VK_FORMAT_R32_SFLOAT;
            break;
        case pinut::resources::TextureFormat::R32G32_UINT:
            return VK_FORMAT_R32G32_UINT;
            break;
        case pinut::resources::TextureFormat::R32G32_SINT:
            return VK_FORMAT_R32G32_SINT;
            break;
        case pinut::resources::TextureFormat::R32G32_SFLOAT:
            return VK_FORMAT_R32G32_SFLOAT;
            break;
        case pinut::resources::TextureFormat::R32G32B32_UINT:
            return VK_FORMAT_R32G32B32_UINT;
            break;
        case pinut::resources::TextureFormat::R32G32B32_SINT:
            return VK_FORMAT_R32G32B32_SINT;
            break;
        case pinut::resources::TextureFormat::R32G32B32_SFLOAT:
            return VK_FORMAT_R32G32B32_SFLOAT;
            break;
        case pinut::resources::TextureFormat::R32G32B32A32_UINT:
            return VK_FORMAT_R32G32B32A32_UINT;
            break;
        case pinut::resources::TextureFormat::R32G32B32A32_SINT:
            return VK_FORMAT_R32G32B32A32_SINT;
            break;
        case pinut::resources::TextureFormat::R32G32B32A32_SFLOAT:
            return VK_FORMAT_R32G32B32A32_SFLOAT;
            break;
        case pinut::resources::TextureFormat::R64_UINT:
            return VK_FORMAT_R64_UINT;
            break;
        case pinut::resources::TextureFormat::R64_SINT:
            return VK_FORMAT_R64_SINT;
            break;
        case pinut::resources::TextureFormat::R64_SFLOAT:
            return VK_FORMAT_R64_SFLOAT;
            break;
        case pinut::resources::TextureFormat::R64G64_UINT:
            return VK_FORMAT_R64G64_UINT;
            break;
        case pinut::resources::TextureFormat::R64G64_SINT:
            return VK_FORMAT_R64G64_SINT;
            break;
        case pinut::resources::TextureFormat::R64G64_SFLOAT:
            return VK_FORMAT_R64G64_SFLOAT;
            break;
        case pinut::resources::TextureFormat::R64G64B64_UINT:
            return VK_FORMAT_R64G64B64_UINT;
            break;
        case pinut::resources::TextureFormat::R64G64B64_SINT:
            return VK_FORMAT_R64G64B64_SINT;
            break;
        case pinut::resources::TextureFormat::R64G64B64_SFLOAT:
            return VK_FORMAT_R64G64B64_SFLOAT;
            break;
        case pinut::resources::TextureFormat::R64G64B64A64_UINT:
            return VK_FORMAT_R64G64B64A64_UINT;
            break;
        case pinut::resources::TextureFormat::R64G64B64A64_SINT:
            return VK_FORMAT_R64G64B64A64_SINT;
            break;
        case pinut::resources::TextureFormat::R64G64B64A64_SFLOAT:
            return VK_FORMAT_R64G64B64A64_SFLOAT;
            break;
        case pinut::resources::TextureFormat::D32_SFLOAT:
            return VK_FORMAT_D32_SFLOAT;
            break;
        case pinut::resources::TextureFormat::D16_UNORM_S8_UINT:
            return VK_FORMAT_D16_UNORM_S8_UINT;
            break;
        case pinut::resources::TextureFormat::D24_UNORM_S8_UINT:
            return VK_FORMAT_D24_UNORM_S8_UINT;
            break;
        case pinut::resources::TextureFormat::D32_SFLOAT_S8_UINT:
            return VK_FORMAT_D32_SFLOAT_S8_UINT;
            break;
        default:
        case pinut::resources::TextureFormat::COUNT:
            return VK_FORMAT_UNDEFINED;
            break;
    }
}
VkImageType get_texture_type(resources::TextureType type)
{
    switch (type)
    {
        case pinut::resources::TextureType::TEXTURE_1D:
            return VK_IMAGE_TYPE_1D;
            break;
        default:
        case pinut::resources::TextureType::TEXTURE_2D:
            return VK_IMAGE_TYPE_2D;
            break;
        case pinut::resources::TextureType::TEXTURE_3D:
            return VK_IMAGE_TYPE_3D;
            break;
        case pinut::resources::TextureType::COUNT:
            return VK_IMAGE_TYPE_MAX_ENUM;
            break;
    }
}
VkImageViewType get_texture_view_type(resources::TextureViewType type)
{
    switch (type)
    {
        case pinut::resources::TextureViewType::TEXTURE_1D:
            return VK_IMAGE_VIEW_TYPE_1D;
            break;
        case pinut::resources::TextureViewType::TEXTURE_2D:
            return VK_IMAGE_VIEW_TYPE_2D;
            break;
        case pinut::resources::TextureViewType::TEXTURE_3D:
            return VK_IMAGE_VIEW_TYPE_3D;
            break;
        case pinut::resources::TextureViewType::TEXTURE_1D_ARRAY:
            return VK_IMAGE_VIEW_TYPE_1D_ARRAY;
            break;
        case pinut::resources::TextureViewType::TEXTURE_2D_ARRAY:
            return VK_IMAGE_VIEW_TYPE_2D_ARRAY;
            break;
        case pinut::resources::TextureViewType::TEXTURE_CUBE:
            return VK_IMAGE_VIEW_TYPE_CUBE;
            break;
        case pinut::resources::TextureViewType::TEXTURE_CUBE_ARRAY:
            return VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
            break;
        default:
        case pinut::resources::TextureViewType::COUNT:
            return VK_IMAGE_VIEW_TYPE_MAX_ENUM;
            break;
    }
}
} // namespace vulkan
} // namespace pinut
