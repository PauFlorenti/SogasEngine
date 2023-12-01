#pragma once

namespace pinut
{
namespace vulkan
{
namespace vkinit
{
VkCommandPoolCreateInfo command_pool_create_info(u32                      queue_family,
                                                 VkCommandPoolCreateFlags flags = 0);

VkCommandBufferAllocateInfo command_buffer_allocate_info(
  VkCommandPool        pool,
  u32                  count = 1,
  VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

VkFramebufferCreateInfo framebuffer_create_info(VkRenderPass renderpass, VkExtent2D extent);

VkFenceCreateInfo fence_create_info(VkFenceCreateFlags flags = 0);

VkSemaphoreCreateInfo semaphore_create_info(VkSemaphoreCreateFlags flags = 0);

VkSubmitInfo submit_info(VkCommandBuffer* cmd);

VkPresentInfoKHR present_info();

VkBufferCreateInfo buffer_info(const u32 size, VkBufferUsageFlags usage);

} // namespace vkinit
} // namespace vulkan
} // namespace pinut
