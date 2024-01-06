#pragma once

#include <vulkan/vulkan.h>

namespace pinut
{
namespace vulkan
{
struct VulkanContext
{
    VkDevice         device         = VK_NULL_HANDLE;
    VkInstance       instance       = VK_NULL_HANDLE;
    VkPhysicalDevice gpu            = VK_NULL_HANDLE;
    VkQueue          graphics_queue = VK_NULL_HANDLE;
    VkRenderPass     render_pass;
    u32              graphics_queue_index;
    void*            window_handle;
};

struct ImguiState
{
    VkDescriptorPool descriptor_pool = VK_NULL_HANDLE;
    VkDevice         device          = VK_NULL_HANDLE;
    VkPhysicalDevice gpu             = VK_NULL_HANDLE;
};

void init_imgui(const VulkanContext& context);
void render_imgui(const VkCommandBuffer& cmd);
void shutdown_imgui();
} // namespace vulkan
} // namespace pinut
