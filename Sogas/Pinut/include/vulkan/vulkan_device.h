#pragma once

#include <render_device.h>
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

    resources::BufferHandle  create_buffer(const resources::BufferDescriptor& descriptor) override;
    resources::TextureHandle create_texture(const resources::TextureDescriptor& descriptor) override;

    void destroy_buffer(resources::BufferHandle handle) override;
    void destroy_texture(resources::TextureHandle handle) override;

  private:
    bool create_instance();
    void setup_debug_messenger();
    void pick_physical_device();
    std::vector<VkDeviceQueueCreateInfo> get_queues();
    void create_device();

    VkInstance                 vulkan_instance = VK_NULL_HANDLE;
    VkDevice                   handle_device   = VK_NULL_HANDLE;
    VkPhysicalDevice           physical_device = VK_NULL_HANDLE;
    VkPhysicalDeviceProperties physical_device_properties;

#ifdef _DEBUG
    VkDebugUtilsMessengerEXT debug_messenger = VK_NULL_HANDLE;
#endif

    std::vector<VkQueueFamilyProperties> queue_family_properties;
    u32                                  graphics_family = VK_QUEUE_FAMILY_IGNORED;
    u32                                  present_family  = VK_QUEUE_FAMILY_IGNORED;
    u32                                  transfer_family = VK_QUEUE_FAMILY_IGNORED;
    VkQueue                              graphics_queue  = VK_NULL_HANDLE;
    VkQueue                              present_queue   = VK_NULL_HANDLE;
    VkQueue                              transfer_queue  = VK_NULL_HANDLE;
    u32                                  frame_count     = 0; // Number of frames since the beginning of the application.
};
} // namespace vulkan
} // namespace pinut
