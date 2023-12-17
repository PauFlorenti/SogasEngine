#pragma once

#include <vulkan/vulkan.h>

namespace pinut
{
namespace vulkan
{
struct SwapchainSupportDetails
{
    VkSurfaceCapabilitiesKHR        surface_capabilities{};
    std::vector<VkSurfaceFormatKHR> surface_formats;
    std::vector<VkPresentModeKHR>   surface_present_modes;
};

struct Swapchain
{
    VkDevice                 device      = VK_NULL_HANDLE;
    VkSwapchainKHR           swapchain   = VK_NULL_HANDLE;
    VkExtent2D               extent      = {0, 0};
    u32                      image_count = 0;
    VkSurfaceFormatKHR       surface_format{};
    VkPresentModeKHR         present_mode = VK_PRESENT_MODE_MAX_ENUM_KHR;
    VkSurfaceFormatKHR       format_surface{};
    std::vector<VkImage>     images;
    std::vector<VkImageView> image_views;

    std::vector<VkImage>     get_images();
    std::vector<VkImageView> get_image_views();
};

class VulkanSwapchainBuilder
{
  public:
    VulkanSwapchainBuilder() = delete;
    explicit VulkanSwapchainBuilder(VkDevice         device,
                                    VkPhysicalDevice physical_device,
                                    VkSurfaceKHR     surface,
                                    u32              graphics_family_index,
                                    u32              present_family_index);

    // Set the desired extent for the swapchain. By default, the swapchain will use the extent of the window.
    VulkanSwapchainBuilder& set_desired_extent(const u32 width, const u32 height);

    // Set the desired format for the swapchain.
    VulkanSwapchainBuilder& set_desired_format(VkSurfaceFormatKHR surface_format);
    // If surface format is not valid or not set, it will take the default value of
    // {VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}
    VulkanSwapchainBuilder& use_default_format();

    // Set the desired present mode for the swapchain.
    VulkanSwapchainBuilder& set_desired_present_mode(VkPresentModeKHR present_mode);
    // Use default present mode if not provided. {VK_PRESENT_MODE_MAILBOX_KHR}
    VulkanSwapchainBuilder& use_default_present_mode();

    // Set the bitmask of the image usage for the acquired swapchain images.
    VulkanSwapchainBuilder& set_image_usage_flags(VkImageUsageFlags usage_flags);
    // Use the default image usage. The default value is VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
    VulkanSwapchainBuilder& use_default_image_usage_flags();

    // Sets the desired minimum image count for the swapchain.
    VulkanSwapchainBuilder& set_desired_min_image_count(u32 min_image_count);
    // Sets the required minimum image count for the swapchain.
    // May throw an error if the required minimum image count is not available by the engine.
    VulkanSwapchainBuilder& set_required_min_image_count(u32 min_image_count);

    Swapchain build();

  private:
    void add_desired_formats(std::vector<VkSurfaceFormatKHR>& formats) const;
    void add_desired_present_modes(std::vector<VkPresentModeKHR>& present_modes) const;

    struct SwapchainInfo
    {
        VkDevice                        device{VK_NULL_HANDLE};
        VkPhysicalDevice                physical_device{VK_NULL_HANDLE};
        VkSurfaceKHR                    surface{VK_NULL_HANDLE};
        std::vector<VkSurfaceFormatKHR> desired_formats;
        std::vector<VkPresentModeKHR>   desired_present_modes;
        VkImageUsageFlags               image_usage_flags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        u32                             desired_width     = 512;
        u32                             desired_height    = 512;
        u32                             min_image_count   = 0;
        u32                             required_min_image_count = 0;
        u32                             graphics_queue_index     = 0;
        u32                             present_queue_index      = 0;
        VkSwapchainKHR                  old_swapchain{VK_NULL_HANDLE};
    } info;
};
} // namespace vulkan
} // namespace pinut
