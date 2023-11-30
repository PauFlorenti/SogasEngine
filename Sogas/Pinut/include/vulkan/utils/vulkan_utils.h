#pragma once

#include <resources/pipeline.h>

namespace pinut
{
namespace vulkan
{
VkCullModeFlags get_cull_mode_flag(resources::CullMode mode);

VkFrontFace get_front_face_flag(resources::FrontFace face);

VkPolygonMode get_cull_mode_flag(resources::FillMode mode);
} // namespace vulkan
} // namespace pinut
