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
    static void create_swapchain_renderpass(VkDevice                  device,
                                            VkFormat                  format,
                                            VkRect2D                  extent,
                                            std::vector<VkImageView>& views);

    VkRenderPass handle;

    u16 width  = 0;
    u16 height = 0;

    resources::RenderPassType type;

    const char* name = nullptr;
};
} // namespace vulkan
} // namespace pinut
