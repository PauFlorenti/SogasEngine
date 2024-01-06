#pragma once

#include <render_device.h>
#include <resources/commandbuffer.h>
#include <resources/resource_pool.h>
#include <resources/resources.h>
#include <resources/shader_state.h>
#include <vulkan/utils/vulkan_commandbuffer.h>
#include <vulkan/utils/vulkan_render_pass.h>
#include <vulkan/utils/vulkan_shader_loader.h>
#include <vulkan/utils/vulkan_swapchain.h>

namespace sogas
{
struct StackAllocator;
} // namespace sogas

namespace pinut
{
namespace vulkan
{
struct VulkanBuffer
{
    VkBuffer       buffer;
    VkDeviceMemory memory;
};

struct VulkanTexture
{
    VkImage        image;
    VkImageView    image_view;
    VkFormat       format;
    VkSampler      sampler;
    VkDeviceMemory memory;
};

struct VulkanDescriptorSet
{
    VkDescriptorSet descriptor_set = VK_NULL_HANDLE;
};

struct VulkanDescriptorSetLayout
{
    VkDescriptorSetLayout layout = VK_NULL_HANDLE;

    resources::DescriptorSetBindingDescriptor* bindings             = nullptr;
    u16                                        bindings_count       = 0;
    u8                                         descriptor_set_index = 0;
};

struct VulkanPipeline
{
    VkPipeline          pipeline        = VK_NULL_HANDLE;
    VkPipelineLayout    pipeline_layout = VK_NULL_HANDLE;
    VkPipelineBindPoint bind_point      = VK_PIPELINE_BIND_POINT_GRAPHICS; // Graphics as default.
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
    resources::DescriptorSetLayoutHandle create_descriptor_set_layout(
      const resources::DescriptorSetLayoutDescriptor& descriptor) override;
    resources::DescriptorSetHandle create_descriptor_set(
      const resources::DescriptorSetDescriptor& descriptor) override;

    // TODO should return a handle for future references outside.
    void create_pipeline(const resources::PipelineDescriptor& descriptor) override;

    void begin_frame() override;
    void end_frame() override;

    resources::CommandBuffer* get_command_buffer(bool begin) override;

    void* map_buffer(const resources::BufferHandle buffer_id,
                     const u32                     size,
                     const u32                     offset = 0) override;
    void  unmap_buffer(const resources::BufferHandle buffer_id) override;

    void copy_buffer(const resources::BufferHandle src_buffer_id,
                     const resources::BufferHandle dst_buffer_id,
                     const u32                     size,
                     const u32                     src_offset = 0,
                     const u32                     dst_offset = 0) override;

    void copy_buffer_to_image(const resources::BufferHandle  buffer_handle,
                              const resources::TextureHandle texture_handle,
                              const u32                      width,
                              const u32                      height) override;

    void destroy_buffer(resources::BufferHandle handle) override;
    void destroy_texture(resources::TextureHandle handle) override;
    void destroy_descriptor_set(resources::DescriptorSetHandle handle) override;
    void destroy_descriptor_set_layout(resources::DescriptorSetLayoutHandle handle) override;

    void destroy_buffer_immediate(resources::ResourceHandle handle) override;
    void destroy_texture_immediate(resources::ResourceHandle handle) override;
    void destroy_descriptor_set_immediate(resources::ResourceHandle handle) override;
    void destroy_descriptor_set_layout_immediate(resources::ResourceHandle handle) override;

    void render_menu_debug(resources::CommandBuffer& cmd) override;

    void destroy_pending_resources();

    static VkCommandBuffer begin_single_use_command_buffer(const VkDevice&      device,
                                                           const VkCommandPool& command_pool);
    static void            end_single_use_command_buffer(const VkDevice&      device,
                                                         const VkCommandPool& command_pool,
                                                         const VkQueue&       queue,
                                                         VkCommandBuffer      cmd);

    static std::map<std::string, VulkanShaderState> shaders;
    static std::map<std::string, VulkanPipeline>    pipelines;
    static std::map<std::string, VulkanRenderPass>  render_passes;

    static const u32 MAX_SWAPCHAIN_IMAGES = 3;

    VkDevice      device                             = VK_NULL_HANDLE;
    VkFramebuffer framebuffers[MAX_SWAPCHAIN_IMAGES] = {VK_NULL_HANDLE};
    u32           swapchain_index                    = 0;
    u32           current_frame                      = 0;

    VkExtent2D get_swapchain_extent() const
    {
        return extent;
    }

    std::string get_swapchain_pass() const
    {
        return "Swapchain_renderpass";
    };

    // Access resources
    VulkanBuffer*              access_buffer(resources::ResourceHandle handle);
    VulkanTexture*             access_texture(resources::ResourceHandle handle);
    VulkanDescriptorSet*       access_descriptor_set(resources::ResourceHandle handle);
    VulkanDescriptorSetLayout* access_descriptor_set_layout(resources::ResourceHandle handle);

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
    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
    VkInstance       vulkan_instance = VK_NULL_HANDLE;
    VkSurfaceKHR     vulkan_surface  = VK_NULL_HANDLE;

    VkPhysicalDeviceProperties           physical_device_properties;
    VkPhysicalDeviceMemoryProperties     physical_device_memory_properties;
    VkFormatProperties                   physical_device_format_properties;
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

    std::vector<resources::ResourceDeletion> deletion_queue;

    // Swapchain variables
    Swapchain swapchain;

    std::vector<VkImage>     swapchain_images;
    std::vector<VkImageView> swapchain_image_views;
    resources::TextureHandle depth_texture;
    VkFormat                 depth_texture_format;

    VulkanCommandBuffer command_buffers[MAX_SWAPCHAIN_IMAGES];

    VkDescriptorPool descriptor_pool;

    static const u16 DEFAULT_RESOURCES_COUNT = 128;

    resources::ResourcePool buffers;
    resources::ResourcePool textures;
    resources::ResourcePool descriptor_sets;
    resources::ResourcePool descriptor_set_layouts;

#ifdef _DEBUG
    VkDebugUtilsMessengerEXT debug_messenger = VK_NULL_HANDLE;
#endif

    // Class helpers
    VulkanShaderLoader vulkan_shader_loader;
    // Default triangle drawing. Probably gonna be removed from here.
    //! Temporal
    VkCommandPool command_pool;

    VkSemaphore present_semaphores[MAX_SWAPCHAIN_IMAGES];
    VkSemaphore render_semaphores[MAX_SWAPCHAIN_IMAGES];
    VkFence     render_fences[MAX_SWAPCHAIN_IMAGES];

    VkRenderPass render_pass;
    //! End temporal block
};
} // namespace vulkan
} // namespace pinut
