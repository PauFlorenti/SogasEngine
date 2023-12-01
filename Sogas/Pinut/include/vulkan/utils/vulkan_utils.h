#pragma once

#include <resources/buffer.h>
#include <resources/pipeline.h>
#include <resources/resources.h>

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
} // namespace vulkan
} // namespace pinut
