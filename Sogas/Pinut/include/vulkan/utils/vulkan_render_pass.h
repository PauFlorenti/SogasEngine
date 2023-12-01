#pragma once

#include <resources/renderpass.h>
#include <vulkan/vulkan.h>

namespace pinut
{
namespace vulkan
{
class VulkanRenderPass
{
  public:
    VkRenderPass handle;

    u16 width  = 0;
    u16 height = 0;

    resources::RenderPassType type;

    const char* name = nullptr;
};
} // namespace vulkan
} // namespace pinut
