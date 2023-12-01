#include "pch.hpp"

#include <vulkan/utils/vulkan_initializers.h>

namespace pinut
{
namespace vulkan
{
namespace vkinit
{
VkCommandPoolCreateInfo command_pool_create_info(u32 queue_family, VkCommandPoolCreateFlags flags)
{
    VkCommandPoolCreateInfo info = {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
    info.queueFamilyIndex        = queue_family;
    info.flags                   = flags;

    return info;
}

VkCommandBufferAllocateInfo command_buffer_allocate_info(VkCommandPool        pool,
                                                         u32                  count,
                                                         VkCommandBufferLevel level)
{
    VkCommandBufferAllocateInfo info = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    info.commandBufferCount          = count;
    info.commandPool                 = pool;
    info.level                       = level;

    return info;
}

VkFramebufferCreateInfo framebuffer_create_info(VkRenderPass renderpass, VkExtent2D extent)
{
    VkFramebufferCreateInfo info = {VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
    info.renderPass              = renderpass;
    info.width                   = extent.width;
    info.height                  = extent.height;
    info.attachmentCount         = 1;
    info.layers                  = 1;

    return info;
}

VkFenceCreateInfo fence_create_info(VkFenceCreateFlags flags)
{
    VkFenceCreateInfo info = {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    info.flags             = flags;

    return info;
}

VkSemaphoreCreateInfo semaphore_create_info(VkSemaphoreCreateFlags flags)
{
    VkSemaphoreCreateInfo info = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    info.flags                 = flags;

    return info;
}

VkSubmitInfo submit_info(VkCommandBuffer* cmd)
{
    VkSubmitInfo info       = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
    info.commandBufferCount = 1;
    info.pCommandBuffers    = cmd;

    return info;
}

VkPresentInfoKHR present_info()
{
    VkPresentInfoKHR info = {VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};

    return info;
}

VkBufferCreateInfo buffer_info(const u32 size, VkBufferUsageFlags usage)
{
    VkBufferCreateInfo info = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    info.size               = size;
    info.sharingMode        = VK_SHARING_MODE_EXCLUSIVE;
    info.usage              = usage;

    return info;
}

} // namespace vkinit
} // namespace vulkan
} // namespace pinut
