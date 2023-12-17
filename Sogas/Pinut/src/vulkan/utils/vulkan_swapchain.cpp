#include "pch.hpp"

#include <vulkan/utils/vulkan_swapchain.h>

namespace
{
static pinut::vulkan::SwapchainSupportDetails
query_swapchain_support(VkPhysicalDevice physical_device, VkSurfaceKHR vulkan_surface)
{
    pinut::vulkan::SwapchainSupportDetails swapchain_support_details;
    auto                                   ok =
      vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device,
                                                vulkan_surface,
                                                &swapchain_support_details.surface_capabilities);

    if (ok != VK_SUCCESS)
    {
        return {};
    }

    u32 format_count = 0;
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device,
                                                  vulkan_surface,
                                                  &format_count,
                                                  nullptr));

    if (format_count != 0)
    {
        swapchain_support_details.surface_formats.resize(format_count);
        VK_CHECK(
          vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device,
                                               vulkan_surface,
                                               &format_count,
                                               swapchain_support_details.surface_formats.data()));
    }

    u32 present_count = 0;
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device,
                                                       vulkan_surface,
                                                       &present_count,
                                                       nullptr));

    if (present_count != 0)
    {
        swapchain_support_details.surface_present_modes.resize(present_count);
        VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(
          physical_device,
          vulkan_surface,
          &present_count,
          swapchain_support_details.surface_present_modes.data()));
    }

    return swapchain_support_details;
}
} // namespace

namespace pinut
{
namespace vulkan
{
std::vector<VkImage> Swapchain::get_images()
{
    vkGetSwapchainImagesKHR(device, swapchain, &image_count, nullptr);
    images.resize(image_count);
    vkGetSwapchainImagesKHR(device, swapchain, &image_count, images.data());

    return images;
}

std::vector<VkImageView> Swapchain::get_image_views()
{
    image_views.resize(image_count);

    for (u32 index = 0; index < image_count; ++index)
    {
        VkImageViewCreateInfo info           = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
        info.image                           = images[index];
        info.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
        info.format                          = surface_format.format;
        info.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        info.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        info.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        info.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        info.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        info.subresourceRange.baseMipLevel   = 0;
        info.subresourceRange.levelCount     = 1;
        info.subresourceRange.baseArrayLayer = 0;
        info.subresourceRange.layerCount     = 1;

        if (vkCreateImageView(device, &info, nullptr, &image_views[index]) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create swapchain image view.");
        }
    }

    return image_views;
}

VulkanSwapchainBuilder::VulkanSwapchainBuilder(VkDevice         device,
                                               VkPhysicalDevice physical_device,
                                               VkSurfaceKHR     surface,
                                               u32              graphics_family_index,
                                               u32              present_family_index)
{
    info.device               = device;
    info.physical_device      = physical_device;
    info.surface              = surface;
    info.graphics_queue_index = graphics_family_index;
    info.present_queue_index  = present_family_index;
}

Swapchain VulkanSwapchainBuilder::build()
{
    Swapchain swapchain;
    swapchain.device = info.device;

    const auto swapchain_support = query_swapchain_support(info.physical_device, info.surface);

    if (swapchain_support.surface_formats.empty() ||
        swapchain_support.surface_present_modes.empty())
    {
        throw std::runtime_error("Swapchain surface formats and present modes are not valid.");
    }

    {
        bool found = false;
        for (const auto& desired_format : info.desired_formats)
        {
            for (const auto& available_format : swapchain_support.surface_formats)
            {
                if (desired_format.format == available_format.format &&
                    desired_format.colorSpace == available_format.colorSpace)
                {
                    swapchain.surface_format = available_format;
                    found                    = true;
                    break;
                }
            }

            if (found)
            {
                break;
            }
        }
    }

    {
        bool found = false;
        for (const auto& desired_mode : info.desired_present_modes)
        {
            for (const auto& available_mode : swapchain_support.surface_present_modes)
            {
                if (desired_mode == available_mode)
                {
                    swapchain.present_mode = available_mode;
                    found                  = true;
                    break;
                }
            }

            if (found)
            {
                break;
            }
        }
    }

    if (swapchain_support.surface_capabilities.currentExtent.width != UINT_MAX)
    {
        swapchain.extent = swapchain_support.surface_capabilities.currentExtent;
    }
    else
    {
        VkExtent2D current_extend = {info.desired_width, info.desired_height};

        current_extend.width =
          std::clamp(current_extend.width,
                     swapchain_support.surface_capabilities.minImageExtent.width,
                     swapchain_support.surface_capabilities.maxImageExtent.width);

        current_extend.height =
          std::clamp(current_extend.height,
                     swapchain_support.surface_capabilities.minImageExtent.height,
                     swapchain_support.surface_capabilities.maxImageExtent.height);

        swapchain.extent = current_extend;
    }

    swapchain.image_count = swapchain_support.surface_capabilities.minImageCount + 1;

    if (swapchain_support.surface_capabilities.maxImageCount > 0 &&
        swapchain.image_count > swapchain_support.surface_capabilities.maxImageCount)
    {
        swapchain.image_count = swapchain_support.surface_capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swapchain_info = {VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
    swapchain_info.surface                  = info.surface;
    swapchain_info.minImageCount            = swapchain.image_count;
    swapchain_info.imageFormat              = swapchain.surface_format.format;
    swapchain_info.imageColorSpace          = swapchain.surface_format.colorSpace;
    swapchain_info.imageExtent              = swapchain.extent;
    swapchain_info.imageArrayLayers         = 1;
    swapchain_info.imageUsage               = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    if (info.graphics_queue_index == info.present_queue_index)
    {
        swapchain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }
    else
    {
        u32 indices[2]                  = {info.graphics_queue_index, info.present_queue_index};
        swapchain_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchain_info.queueFamilyIndexCount = 2;
        swapchain_info.pQueueFamilyIndices   = indices;
    }

    swapchain_info.preTransform   = swapchain_support.surface_capabilities.currentTransform;
    swapchain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_info.presentMode    = swapchain.present_mode;
    swapchain_info.clipped        = VK_TRUE;
    swapchain_info.oldSwapchain   = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(swapchain.device, &swapchain_info, nullptr, &swapchain.swapchain) !=
        VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create the swapchain");
    }

    PDEBUG("Swapchain created!");

    return swapchain;
}

VulkanSwapchainBuilder& VulkanSwapchainBuilder::set_desired_extent(const u32 width,
                                                                   const u32 height)
{
    info.desired_width  = width;
    info.desired_height = height;
    return *this;
}

VulkanSwapchainBuilder& VulkanSwapchainBuilder::set_desired_format(
  VkSurfaceFormatKHR surface_format)
{
    info.desired_formats.insert(info.desired_formats.begin(), surface_format);
    return *this;
}

VulkanSwapchainBuilder& VulkanSwapchainBuilder::use_default_format()
{
    info.desired_formats.clear();
    add_desired_formats(info.desired_formats);
    return *this;
}

VulkanSwapchainBuilder& VulkanSwapchainBuilder::set_desired_present_mode(
  VkPresentModeKHR present_mode)
{
    info.desired_present_modes.insert(info.desired_present_modes.begin(), present_mode);
    return *this;
}

VulkanSwapchainBuilder& VulkanSwapchainBuilder::use_default_present_mode()
{
    info.desired_present_modes.clear();
    add_desired_present_modes(info.desired_present_modes);
    return *this;
}

VulkanSwapchainBuilder& VulkanSwapchainBuilder::set_image_usage_flags(VkImageUsageFlags usage_flags)
{
    info.image_usage_flags = usage_flags;
    return *this;
}

VulkanSwapchainBuilder& VulkanSwapchainBuilder::use_default_image_usage_flags()
{
    info.image_usage_flags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    return *this;
}

VulkanSwapchainBuilder& VulkanSwapchainBuilder::set_desired_min_image_count(u32 min_image_count)
{
    info.min_image_count = min_image_count;
    return *this;
}

VulkanSwapchainBuilder& VulkanSwapchainBuilder::set_required_min_image_count(u32 min_image_count)
{
    info.required_min_image_count = min_image_count;
    return *this;
}

void VulkanSwapchainBuilder::add_desired_formats(std::vector<VkSurfaceFormatKHR>& formats) const
{
    formats.push_back({VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR});
}

void VulkanSwapchainBuilder::add_desired_present_modes(
  std::vector<VkPresentModeKHR>& present_modes) const
{
    present_modes.push_back({VK_PRESENT_MODE_MAILBOX_KHR});
}
} // namespace vulkan
} // namespace pinut
