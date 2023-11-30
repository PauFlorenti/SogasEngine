#include "pch.hpp"

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
} // namespace vulkan
} // namespace pinut
