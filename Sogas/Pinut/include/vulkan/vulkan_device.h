#pragma once

#include <render_device.h>
#include <vulkan/utils/vulkan_pipeline_builder.h>
#include <vulkan/utils/vulkan_shader_loader.h>
#include <vulkan/utils/vulkan_swapchain_builder.h>
#include <vulkan/vulkan.h>

namespace pinut
{
namespace vulkan
{
class VulkanDevice : public GPUDevice
{
  public:
    VulkanDevice() = default;
    ~VulkanDevice() override;

    void init(const DeviceDescriptor& descriptor) override;
    void shutdown() override;
    void resize(u32 width, u32 height);

    //! TODO: temporal function
    void update() override;
    //! end temporal

    resources::BufferHandle  create_buffer(const resources::BufferDescriptor& descriptor) override;
    resources::TextureHandle create_texture(
      const resources::TextureDescriptor& descriptor) override;

    void destroy_buffer(resources::BufferHandle handle) override;
    void destroy_texture(resources::TextureHandle handle) override;

  private:
    bool                                 create_instance();
    void                                 setup_debug_messenger();
    void                                 pick_physical_device();
    std::vector<VkDeviceQueueCreateInfo> get_queues();
    void                                 create_device();
    void                                 create_surface(void* window);

    // Swapchain functions
    void create_swapchain();
    void destroy_swapchain();
    void recreate_swapchain();

    // Window handle
    void* window_handle = nullptr;

    // Basic initialization handles
    VkDevice         device          = VK_NULL_HANDLE;
    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
    VkInstance       vulkan_instance = VK_NULL_HANDLE;
    VkSurfaceKHR     vulkan_surface  = VK_NULL_HANDLE;

    VkPhysicalDeviceProperties           physical_device_properties;
    std::vector<VkQueueFamilyProperties> queue_family_properties;

    VkExtent2D extent{512, 512};
    VkExtent2D new_extent{0, 0};
    bool minimized = false;

    // Queues
    u32 graphics_family = VK_QUEUE_FAMILY_IGNORED;
    u32 present_family  = VK_QUEUE_FAMILY_IGNORED;
    u32 transfer_family = VK_QUEUE_FAMILY_IGNORED;

    VkQueue graphics_queue = VK_NULL_HANDLE;
    VkQueue present_queue  = VK_NULL_HANDLE;
    VkQueue transfer_queue = VK_NULL_HANDLE;

    // Swapchain variables
    Swapchain swapchain;

    std::vector<VkFramebuffer> framebuffers;
    std::vector<VkImage>       swapchain_images;
    std::vector<VkImageView>   swapchain_image_views;

#ifdef _DEBUG
    VkDebugUtilsMessengerEXT debug_messenger = VK_NULL_HANDLE;
#endif

    // Class helpers
    VulkanShaderLoader    vulkan_shader_loader;
    VulkanPipelineBuilder pipeline_builder;

    // Default triangle drawing. Probably gonna be removed from here.
    //! Temporal
    VkShaderModule vertex_shader_module   = VK_NULL_HANDLE;
    VkShaderModule fragment_shader_module = VK_NULL_HANDLE;

    VkCommandPool   command_pool;
    VkCommandBuffer cmd;

    VkSemaphore present_semaphore;
    VkSemaphore render_semaphore;
    VkFence     render_fence;

    VkPipeline       triangle_pipeline;
    VkPipelineLayout triangle_pipeline_layout;
    VkRenderPass     render_pass;
    //! End temporal block
};
} // namespace vulkan
} // namespace pinut
