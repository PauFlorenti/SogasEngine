#pragma once

#include <render_device.h>
#include <resources/commandbuffer.h>
#include <resources/shader_state.h>
#include <vulkan/utils/vulkan_commandbuffer.h>
#include <vulkan/utils/vulkan_pipeline_builder.h>
#include <vulkan/utils/vulkan_render_pass.h>
#include <vulkan/utils/vulkan_shader_loader.h>
#include <vulkan/utils/vulkan_swapchain_builder.h>

namespace pinut
{
namespace vulkan
{

struct VulkanBuffer
{
    VkBuffer       buffer;
    VkDeviceMemory memory;
};

class VulkanDevice : public GPUDevice
{
  public:
    VulkanDevice() = default;
    ~VulkanDevice() override;

    void init(const DeviceDescriptor& descriptor) override;
    void shutdown() override;
    void resize(u32 width, u32 height);

    resources::BufferHandle  create_buffer(const resources::BufferDescriptor& descriptor) override;
    resources::TextureHandle create_texture(
      const resources::TextureDescriptor& descriptor) override;
    resources::RenderPassHandle create_renderpass(
      const resources::RenderPassDescriptor& descriptor) override;
    const u32 create_descriptor_set_layout(
      const resources::DescriptorSetLayoutDescriptor& descriptor) override;
    const u32 create_descriptor_set(const resources::DescriptorSetDescriptor& descriptor) override;

    // TODO should return a handle for future references outside.
    void create_pipeline(const resources::PipelineDescriptor& descriptor) override;

    void begin_frame() override;
    void end_frame() override;

    resources::CommandBuffer* get_command_buffer(bool begin) override;

    void* map_buffer(const u32 buffer_id, const u32 size) override;
    void  unmap_buffer(const u32 buffer_id) override;

    void copy_buffer(const u32 src_buffer_id,
                     const u32 dst_buffer_id,
                     const u32 size,
                     const u32 src_offset = 0,
                     const u32 dst_offset = 0) override;

    void destroy_buffer(resources::BufferHandle handle) override;
    void destroy_texture(resources::TextureHandle handle) override;

    static std::map<std::string, VulkanShaderState> shaders;
    static std::map<std::string, VulkanPipeline>    pipelines;
    static std::map<std::string, VulkanRenderPass>  render_passes;
    static std::map<u32, VulkanBuffer>              buffers;
    static std::map<u32, VkDescriptorSetLayout>     descriptor_set_layouts;
    static std::map<u32, VkDescriptorSet>           descriptor_sets;

    // TODO Do not use this values as buffer indexes.
    u32 buffer_index = INVALID_ID;

    static const u32 MAX_SWAPCHAIN_IMAGES = 3;

    VkFramebuffer framebuffers[MAX_SWAPCHAIN_IMAGES];
    u32           swapchain_index = 0;
    u32           current_frame   = 0;

    VkExtent2D get_swapchain_extent() const
    {
        return extent;
    }

    std::string get_swapchain_pass() const
    {
        return "Swapchain_renderpass";
    };

  private:
    bool                                 create_instance();
    void                                 setup_debug_messenger();
    void                                 pick_physical_device();
    std::vector<VkDeviceQueueCreateInfo> get_queues();
    void                                 create_device();
    void                                 create_surface(void* window);

    u32 find_memory_type(const u32 type_filter, const VkMemoryPropertyFlags property_flags);

    // Swapchain functions
    void create_swapchain();
    void destroy_swapchain();
    void recreate_swapchain();

    // internals
    void create_vulkan_buffer(const u32             size,
                              VkBufferUsageFlags    usage_flags,
                              VkMemoryPropertyFlags memory_property_flags,
                              VulkanBuffer*         buffer);

    // Window handle
    void* window_handle = nullptr;

    // Basic initialization handles
    VkDevice         device          = VK_NULL_HANDLE;
    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
    VkInstance       vulkan_instance = VK_NULL_HANDLE;
    VkSurfaceKHR     vulkan_surface  = VK_NULL_HANDLE;

    VkPhysicalDeviceProperties           physical_device_properties;
    VkPhysicalDeviceMemoryProperties     physical_device_memory_properties;
    std::vector<VkQueueFamilyProperties> queue_family_properties;

    VkExtent2D extent{512, 512};
    VkExtent2D new_extent{0, 0};
    bool       minimized = false;

    // Queues
    u32 graphics_family = VK_QUEUE_FAMILY_IGNORED;
    u32 present_family  = VK_QUEUE_FAMILY_IGNORED;
    u32 transfer_family = VK_QUEUE_FAMILY_IGNORED;

    VkQueue graphics_queue = VK_NULL_HANDLE;
    VkQueue present_queue  = VK_NULL_HANDLE;
    VkQueue transfer_queue = VK_NULL_HANDLE;

    // Swapchain variables
    Swapchain swapchain;

    std::vector<VkImage>     swapchain_images;
    std::vector<VkImageView> swapchain_image_views;

    VulkanCommandBuffer command_buffer[MAX_SWAPCHAIN_IMAGES];

    VkDescriptorPool descriptor_pool;

#ifdef _DEBUG
    VkDebugUtilsMessengerEXT debug_messenger = VK_NULL_HANDLE;
#endif

    // Class helpers
    VulkanShaderLoader vulkan_shader_loader;

    //VulkanRenderPass* swapchain_renderpass = nullptr;

    // Default triangle drawing. Probably gonna be removed from here.
    //! Temporal
    VkCommandPool   command_pool;
    VkCommandBuffer cmd;

    VkSemaphore present_semaphores[MAX_SWAPCHAIN_IMAGES];
    VkSemaphore render_semaphores[MAX_SWAPCHAIN_IMAGES];
    VkFence     render_fences[MAX_SWAPCHAIN_IMAGES];

    VkRenderPass render_pass;
    //! End temporal block
};
} // namespace vulkan
} // namespace pinut
