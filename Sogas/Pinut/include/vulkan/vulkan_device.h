#pragma once

#include <render_device.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_pipeline_builder.h>
#include <vulkan/vulkan_shader_loader.h>

namespace pinut
{
namespace vulkan
{

struct SwapchainSupportDetails
{
    VkSurfaceCapabilitiesKHR        surface_capabilities;
    std::vector<VkSurfaceFormatKHR> surface_formats;
    std::vector<VkPresentModeKHR>   surface_present_modes;
};

class VulkanDevice : public GPUDevice
{
  public:
    VulkanDevice() = default;
    ~VulkanDevice() override;

    void init(const DeviceDescriptor& descriptor) override;
    void shutdown() override;

    void update() override;

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

    // Swapchain
    void create_swapchain(const DeviceDescriptor& descriptor);
    void destroy_swapchain();

    VkDevice         handle_device   = VK_NULL_HANDLE;
    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
    VkInstance       vulkan_instance = VK_NULL_HANDLE;
    VkSurfaceKHR     vulkan_surface  = VK_NULL_HANDLE;
    VkSwapchainKHR   swapchain       = VK_NULL_HANDLE;

    VkFormat swapchain_format;

    VkPhysicalDeviceProperties physical_device_properties;

#ifdef _DEBUG
    VkDebugUtilsMessengerEXT debug_messenger = VK_NULL_HANDLE;
#endif

    std::vector<VkQueueFamilyProperties> queue_family_properties;

    u32 graphics_family = VK_QUEUE_FAMILY_IGNORED;
    u32 present_family  = VK_QUEUE_FAMILY_IGNORED;
    u32 transfer_family = VK_QUEUE_FAMILY_IGNORED;

    VkQueue graphics_queue = VK_NULL_HANDLE;
    VkQueue present_queue  = VK_NULL_HANDLE;
    VkQueue transfer_queue = VK_NULL_HANDLE;

    u32 frame_count           = 0; // Number of frames since the beginning of the application.
    u32 swapchain_image_count = 0;

    std::vector<VkFramebuffer> framebuffers;
    std::vector<VkImage>       swapchain_images;
    std::vector<VkImageView>   swapchain_image_views;

    VulkanShaderLoader vulkan_shader_loader;
    VkShaderModule     vertex_shader_module   = VK_NULL_HANDLE;
    VkShaderModule     fragment_shader_module = VK_NULL_HANDLE;

    VulkanPipelineBuilder pipeline_builder;

    VkCommandPool   command_pool;
    VkCommandBuffer cmd;

    VkSemaphore present_semaphore;
    VkSemaphore render_semaphore;
    VkFence     render_fence;

    VkPipeline       triangle_pipeline;
    VkPipelineLayout triangle_pipeline_layout;
    VkRenderPass     render_pass; //! Default render pass ... temporal
};
} // namespace vulkan
} // namespace pinut
