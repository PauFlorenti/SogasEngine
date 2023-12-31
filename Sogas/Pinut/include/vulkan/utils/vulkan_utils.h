#pragma once

#include <resources/buffer.h>
#include <resources/pipeline.h>
#include <resources/resources.h>
#include <resources/texture.h>

namespace pinut
{
namespace vulkan
{
VkCullModeFlags get_cull_mode_flag(resources::CullMode mode);

VkFrontFace get_front_face_flag(resources::FrontFace face);

VkPolygonMode get_cull_mode_flag(resources::FillMode mode);

VkFormat get_vertex_input_format(resources::VertexInputFormatType format);

VkVertexInputRate get_vertex_input_rate(resources::VertexInputRate input_rate);

VkPrimitiveTopology get_topology(resources::TopologyType topology);

VkBufferUsageFlags get_buffer_usage_flag(resources::BufferType buffer_type);

VkDescriptorType get_descriptor_type(resources::DescriptorType descriptor_type);

VkShaderStageFlags get_shader_stage_flag(resources::ShaderStageType shader_stage);

VkIndexType get_buffer_index_type(resources::BufferIndexType index_type);

VkFormat get_texture_format(resources::TextureFormat format);

VkImageType get_texture_type(resources::TextureType type);

VkImageViewType get_texture_view_type(resources::TextureViewType type);

inline bool has_depth_or_stencil(VkFormat format)
{
    return format >= VK_FORMAT_D16_UNORM && format <= VK_FORMAT_D32_SFLOAT_S8_UINT;
}
} // namespace vulkan
} // namespace pinut
