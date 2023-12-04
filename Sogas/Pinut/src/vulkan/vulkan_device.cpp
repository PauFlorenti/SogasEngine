#include "pch.hpp"

#include <vulkan/utils/vulkan_initializers.h>
#include <vulkan/utils/vulkan_render_pass.h>
#include <vulkan/utils/vulkan_swapchain_builder.h>
#include <vulkan/utils/vulkan_utils.h>
#include <vulkan/vulkan_debug.h>
#include <vulkan/vulkan_device.h>

#include <resources/pipeline.h>
#include <resources/renderpass.h>
#include <resources/shader_state.h>

#ifdef WIN32
#include <windows.h>

#include <vulkan/vulkan_win32.h>
#endif
namespace
{
static const std::vector<const char*> validation_layers = {"VK_LAYER_KHRONOS_validation"};
static const std::vector<const char*> required_instance_extensions = {
#ifdef WIN32
  "VK_KHR_win32_surface",
#endif
  VK_KHR_SURFACE_EXTENSION_NAME};
static const std::vector<const char*> required_device_extensions = {
  VK_KHR_SWAPCHAIN_EXTENSION_NAME};

static bool check_required_extensions_available(
  const std::vector<VkExtensionProperties>& available_extensions)
{
    for (const auto extension_name : required_instance_extensions)
    {
        bool found = false;
        for (const auto& available_extension : available_extensions)
        {
            if (strcmp(extension_name, available_extension.extensionName) == 0)
            {
                found = true;
                break;
            }
        }

        if (!found)
            return false;
    }

    return true;
}

static bool check_device_extension_support(const VkPhysicalDevice physical_device)
{
    u32 extension_count;
    vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, nullptr);
    std::vector<VkExtensionProperties> available_extensions(extension_count);
    vkEnumerateDeviceExtensionProperties(physical_device,
                                         nullptr,
                                         &extension_count,
                                         available_extensions.data());

    std::set<std::string> required_extensions(required_device_extensions.begin(),
                                              required_device_extensions.end());

    for (const auto& extension : available_extensions)
    {
        required_extensions.erase(extension.extensionName);
    }

    return required_extensions.empty();
}

#ifdef _DEBUG
static bool check_validation_layer_support(const std::vector<VkLayerProperties>& available_layers)
{
    for (const auto layer_name : validation_layers)
    {
        bool found = false;
        for (const auto& available_layer : available_layers)
        {
            if (strcmp(layer_name, available_layer.layerName) == 0)
            {
                found = true;
                break;
            }
        }

        if (!found)
            return false;
    }

    return true;
}
#endif

static std::vector<VkExtensionProperties> get_available_instance_extensions()
{
    u32 extensions_count = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, nullptr);

    std::vector<VkExtensionProperties> available_extensions(extensions_count);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, available_extensions.data());

#ifdef _DEBUG
    PDEBUG("Available instance extenions in the system:");
    for (const auto& extension : available_extensions)
    {
        PDEBUG("\t%s", extension.extensionName);
    }
    PDEBUG("");
#endif
    return available_extensions;
}

#ifdef _DEBUG
static std::vector<VkLayerProperties> get_available_instance_layers()
{
    u32 layers_count = 0;
    vkEnumerateInstanceLayerProperties(&layers_count, nullptr);

    std::vector<VkLayerProperties> available_layers(layers_count);
    vkEnumerateInstanceLayerProperties(&layers_count, available_layers.data());

    PDEBUG("Available instance layers in the system:");
    for (const auto& layer : available_layers)
    {
        PDEBUG("\t%s", layer.layerName);
    }
    PDEBUG("");
    return available_layers;
}
#endif
} // namespace
namespace pinut
{
namespace vulkan
{

#ifdef _DEBUG
static constexpr bool enable_validation_layers = true;
#else
static constexpr bool enable_validation_layers = false;
#endif

std::map<std::string, VulkanShaderState> VulkanDevice::shaders;
std::map<std::string, VulkanPipeline>    VulkanDevice::pipelines;
std::map<std::string, VulkanRenderPass>  VulkanDevice::render_passes;
std::map<u32, VkDescriptorSetLayout>     VulkanDevice::descriptor_set_layouts;
std::map<u32, VkDescriptorSet>           VulkanDevice::descriptor_sets;

VulkanDevice::~VulkanDevice()
{
    shutdown();
}

void VulkanDevice::init(const DeviceDescriptor& descriptor)
{
    window_handle = descriptor.window;
    extent        = {descriptor.width, descriptor.height};

    PDEBUG("GPU Device init.");
    create_instance();
    create_surface(window_handle);
    create_device();

    vkGetPhysicalDeviceMemoryProperties(physical_device, &physical_device_memory_properties);

    create_swapchain();

    buffers.init(128, sizeof(VulkanBuffer));
    textures.init(128, sizeof(VulkanTexture));

    // Move temporal render pass here for the swapchain's framebuffers creation
    VkAttachmentDescription color_attachment = {};
    color_attachment.format                  = swapchain.surface_format.format;
    color_attachment.samples                 = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp                  = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp                 = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp           = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp          = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout           = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout             = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference attachment_reference = {};
    attachment_reference.attachment            = 0;
    attachment_reference.layout                = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments    = &attachment_reference;
    subpass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;

    VkRenderPassCreateInfo render_pass_info = {VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
    render_pass_info.attachmentCount        = 1;
    render_pass_info.pAttachments           = &color_attachment;
    render_pass_info.subpassCount           = 1;
    render_pass_info.pSubpasses             = &subpass;

    VK_CHECK(vkCreateRenderPass(device, &render_pass_info, nullptr, &render_pass));

    VulkanRenderPass vulkan_renderpass = {};
    vulkan_renderpass.type             = resources::RenderPassType::SWAPCHAIN;
    vulkan_renderpass.handle           = render_pass;
    vulkan_renderpass.width            = static_cast<u16>(extent.width);
    vulkan_renderpass.height           = static_cast<u16>(extent.height);

    VulkanDevice::render_passes.insert({"Swapchain_renderpass", vulkan_renderpass});

    VkFramebufferCreateInfo framebuffer_info = vkinit::framebuffer_create_info(render_pass, extent);

    const u32 swapchain_images_size = static_cast<u32>(swapchain_images.size());

    for (u32 i = 0; i < swapchain_images_size; ++i)
    {
        framebuffer_info.pAttachments = &swapchain_image_views.at(i);

        VK_CHECK(vkCreateFramebuffer(device, &framebuffer_info, nullptr, &framebuffers[i]));
    }

    // Create command pool
    VkCommandPoolCreateInfo cmd_pool_info =
      vkinit::command_pool_create_info(graphics_family,
                                       VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    VK_CHECK(vkCreateCommandPool(device, &cmd_pool_info, nullptr, &command_pool));

    for (u32 i = 0; i < MAX_SWAPCHAIN_IMAGES; ++i)
    {
        command_buffers[i].device = this;
        auto cmd_alloc_info       = vkinit::command_buffer_allocate_info(command_pool);
        VK_CHECK(vkAllocateCommandBuffers(device, &cmd_alloc_info, &command_buffers[i].cmd));
    }

    // Create semaphores and fences
    for (i32 i = 0; i < MAX_SWAPCHAIN_IMAGES; ++i)
    {
        VkSemaphoreCreateInfo semaphore_info = vkinit::semaphore_create_info();
        VK_CHECK(vkCreateSemaphore(device, &semaphore_info, nullptr, &render_semaphores[i]));
        VK_CHECK(vkCreateSemaphore(device, &semaphore_info, nullptr, &present_semaphores[i]));

        VkFenceCreateInfo fence_info = vkinit::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);
        VK_CHECK(vkCreateFence(device, &fence_info, nullptr, &render_fences[i]));
    }

    // Create Descriptor pool

    VkDescriptorPoolSize pool_size = {};
    pool_size.type                 = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    pool_size.descriptorCount      = MAX_SWAPCHAIN_IMAGES;

    VkDescriptorPoolCreateInfo pool_info = {VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
    pool_info.poolSizeCount              = 1;
    pool_info.pPoolSizes                 = &pool_size;
    pool_info.maxSets                    = static_cast<u32>(MAX_SWAPCHAIN_IMAGES);
    pool_info.flags                      = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

    VK_CHECK(vkCreateDescriptorPool(device, &pool_info, nullptr, &descriptor_pool));
}

void VulkanDevice::shutdown()
{
    VK_CHECK(vkWaitForFences(device, MAX_SWAPCHAIN_IMAGES, render_fences, VK_TRUE, UINT64_MAX));
    VK_CHECK(vkDeviceWaitIdle(device));

    for (auto& descriptor_set : descriptor_sets)
    {
        vkFreeDescriptorSets(device, descriptor_pool, 1, &descriptor_set.second);
    }

    vkDestroyDescriptorPool(device, descriptor_pool, nullptr);

    // Clear shaders
    for (auto& shader : shaders)
    {
        for (auto& module : shader.second)
        {
            vkDestroyShaderModule(device, module.module, nullptr);
        }
    }

    for (auto& descriptor : descriptor_set_layouts)
    {
        vkDestroyDescriptorSetLayout(device, descriptor.second, nullptr);
    }

    for (auto& it : pipelines)
    {
        vkDestroyPipeline(device, it.second.pipeline, nullptr);
        vkDestroyPipelineLayout(device, it.second.pipeline_layout, nullptr);
    }

    for (u32 i = 0; i < MAX_SWAPCHAIN_IMAGES; ++i)
    {
        vkDestroyFence(device, render_fences[i], nullptr);
        vkDestroySemaphore(device, present_semaphores[i], nullptr);
        vkDestroySemaphore(device, render_semaphores[i], nullptr);
    }

    vkDestroyCommandPool(device, command_pool, nullptr);
    destroy_swapchain();
    vkDestroyRenderPass(device, render_pass, nullptr);
    vkDestroySurfaceKHR(vulkan_instance, vulkan_surface, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroyInstance(vulkan_instance, nullptr);
}

void VulkanDevice::resize(u32 width, u32 height)
{
    new_extent = {width, height};

    if (minimized)
    {
        recreate_swapchain();
    }
}

resources::BufferHandle VulkanDevice::create_buffer(const resources::BufferDescriptor& descriptor)
{
    resources::BufferHandle handle{buffers.get_resource()};
    if (handle.id == INVALID_ID)
    {
        return handle;
    }

    VulkanBuffer* buffer = access_buffer(handle);

    VkBufferUsageFlags    usage_flags  = VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
    VkMemoryPropertyFlags memory_flags = VK_MEMORY_PROPERTY_FLAG_BITS_MAX_ENUM;

    switch (descriptor.type)
    {
        case resources::BufferType::STAGING:
            usage_flags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            memory_flags =
              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
            break;
        case resources::BufferType::VERTEX:
            usage_flags  = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            memory_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            break;
        case resources::BufferType::INDEX:
            usage_flags  = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
            memory_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            break;
        case resources::BufferType::UNIFORM:
            usage_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            memory_flags =
              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
            break;
        case resources::BufferType::STORAGE:
            // TODO review storage usage flags.
            usage_flags  = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
            memory_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            break;
        default:
            break;
    }

    create_vulkan_buffer(descriptor.size, usage_flags, memory_flags, buffer);

    if (descriptor.data)
    {
        // TODO Descriptor should provide offset. Data may not be at position 0.
        void* data;
        vkMapMemory(device, buffer->memory, 0, descriptor.size, 0, &data);
        memcpy(data, descriptor.data, descriptor.size);
        vkUnmapMemory(device, buffer->memory);
    }

    return handle;
}

resources::TextureHandle VulkanDevice::create_texture(
  const resources::TextureDescriptor& descriptor)
{
    resources::TextureHandle handle{textures.get_resource()};
    if (handle.id == INVALID_ID)
    {
        return handle;
    }

    if (descriptor.data == nullptr)
    {
        PWARN("No data provided for texture creation.");
        return {INVALID_ID};
    }

    const auto   texture_size = descriptor.width * descriptor.height * descriptor.channel_count;
    VulkanBuffer staging_buffer;
    create_vulkan_buffer(texture_size,
                         VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT,
                         &staging_buffer);

    void* data;
    vkMapMemory(device, staging_buffer.memory, 0, texture_size, 0, &data);
    memcpy(data, descriptor.data, texture_size);
    vkUnmapMemory(device, staging_buffer.memory);

    VkImageCreateInfo info = {VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
    info.mipLevels         = descriptor.mip_levels;
    info.imageType         = VK_IMAGE_TYPE_2D;
    info.extent            = {descriptor.width, descriptor.height, 1};
    info.arrayLayers       = 1;
    info.format            = VK_FORMAT_R8G8B8A8_SRGB;
    info.tiling            = VK_IMAGE_TILING_LINEAR;
    info.initialLayout     = VK_IMAGE_LAYOUT_UNDEFINED;
    info.usage             = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    info.sharingMode       = VK_SHARING_MODE_EXCLUSIVE;
    info.samples           = VK_SAMPLE_COUNT_1_BIT;

    VulkanTexture* texture = access_texture(handle);

    VK_CHECK(vkCreateImage(device, &info, nullptr, &texture->image));

    VkMemoryRequirements memory_requirements;
    vkGetImageMemoryRequirements(device, texture->image, &memory_requirements);

    VkMemoryAllocateInfo allocate_info = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    allocate_info.allocationSize       = memory_requirements.size;
    allocate_info.memoryTypeIndex =
      find_memory_type(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VK_CHECK(vkAllocateMemory(device, &allocate_info, nullptr, &texture->memory));

    vkBindImageMemory(device, texture->image, texture->memory, 0);

    VkCommandBuffer             command_buffer    = VK_NULL_HANDLE;
    VkCommandBufferAllocateInfo cmd_allocate_info = {
      VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    cmd_allocate_info.commandBufferCount = 1;
    cmd_allocate_info.commandPool        = command_pool;
    cmd_allocate_info.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    VK_CHECK(vkAllocateCommandBuffers(device, &cmd_allocate_info, &command_buffer));

    VkCommandBufferBeginInfo begin_info{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(command_buffer, &begin_info);

    VkImageMemoryBarrier barrier{VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
    barrier.image                           = texture->image;
    barrier.oldLayout                       = VK_IMAGE_LAYOUT_UNDEFINED;
    barrier.newLayout                       = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel   = 0;
    barrier.subresourceRange.layerCount     = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.levelCount     = 1;
    barrier.srcAccessMask                   = 0;
    barrier.dstAccessMask                   = VK_ACCESS_TRANSFER_WRITE_BIT;

    vkCmdPipelineBarrier(command_buffer,
                         VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                         VK_PIPELINE_STAGE_TRANSFER_BIT,
                         0,
                         0,
                         nullptr,
                         0,
                         nullptr,
                         1,
                         &barrier);

    VkBufferImageCopy region{};
    region.bufferOffset      = 0;
    region.bufferRowLength   = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel       = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount     = 1;

    region.imageOffset = {0, 0, 0};
    region.imageExtent = {descriptor.width, descriptor.height, 1};

    vkCmdCopyBufferToImage(command_buffer,
                           staging_buffer.buffer,
                           texture->image,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           1,
                           &region);

    barrier.image                           = texture->image;
    barrier.oldLayout                       = VK_IMAGE_LAYOUT_UNDEFINED;
    barrier.newLayout                       = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel   = 0;
    barrier.subresourceRange.layerCount     = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.levelCount     = 1;
    barrier.srcAccessMask                   = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask                   = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(command_buffer,
                         VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                         0,
                         0,
                         nullptr,
                         0,
                         nullptr,
                         1,
                         &barrier);

    vkEndCommandBuffer(command_buffer);

    VkSubmitInfo submit_info       = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers    = &command_buffer;
    vkQueueSubmit(graphics_queue, 1, &submit_info, VK_NULL_HANDLE);

    vkQueueWaitIdle(graphics_queue);
    vkFreeCommandBuffers(device, command_pool, 1, &command_buffer);

    // TODO Use own buffer functions for allocation purposes.
    vkDestroyBuffer(device, staging_buffer.buffer, nullptr);
    vkFreeMemory(device, staging_buffer.memory, nullptr);

    VkImageViewCreateInfo image_view_info           = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    image_view_info.image                           = texture->image;
    image_view_info.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
    image_view_info.format                          = VK_FORMAT_R8G8B8A8_SRGB;
    image_view_info.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    image_view_info.subresourceRange.baseArrayLayer = 0;
    image_view_info.subresourceRange.layerCount     = 1;
    image_view_info.subresourceRange.baseMipLevel   = 0;
    image_view_info.subresourceRange.levelCount     = 1;

    VK_CHECK(vkCreateImageView(device, &image_view_info, nullptr, &texture->image_view));

    //! TODO Make sampler global, not per image.
    VkSamplerCreateInfo sampler_info{};
    sampler_info.sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_info.magFilter               = VK_FILTER_LINEAR;
    sampler_info.minFilter               = VK_FILTER_LINEAR;
    sampler_info.addressModeU            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.addressModeV            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.addressModeW            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.anisotropyEnable        = VK_TRUE;
    sampler_info.maxAnisotropy           = physical_device_properties.limits.maxSamplerAnisotropy;
    sampler_info.borderColor             = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    sampler_info.unnormalizedCoordinates = VK_FALSE;
    sampler_info.compareEnable           = VK_FALSE;
    sampler_info.compareOp               = VK_COMPARE_OP_ALWAYS;
    sampler_info.mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_info.mipLodBias              = 0.0f;
    sampler_info.minLod                  = 0.0f;
    sampler_info.maxLod                  = 0.0f;

    VK_CHECK(vkCreateSampler(device, &sampler_info, nullptr, &texture->sampler))

    return handle;
}

resources::RenderPassHandle VulkanDevice::create_renderpass(
  const resources::RenderPassDescriptor& /*descriptor*/
)
{
    return {};
}

const u32 VulkanDevice::create_descriptor_set_layout(
  const resources::DescriptorSetLayoutDescriptor& descriptor)
{
    ASSERT(descriptor.binding_count > 0);

    std::vector<VkDescriptorSetLayoutBinding> bindings(descriptor.binding_count);
    for (u32 i = 0; i < descriptor.binding_count; ++i)
    {
        auto                         in_binding = descriptor.bindings[i];
        VkDescriptorSetLayoutBinding binding    = {};
        binding.binding                         = in_binding.binding;
        binding.descriptorCount                 = in_binding.count;
        binding.descriptorType                  = get_descriptor_type(in_binding.descriptor_type);
        binding.stageFlags                      = get_shader_stage_flag(in_binding.shader_stage);

        bindings.at(i) = binding;
    }

    VkDescriptorSetLayoutCreateInfo descriptor_set_layout_info = {
      VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
    descriptor_set_layout_info.bindingCount = static_cast<u32>(bindings.size());
    descriptor_set_layout_info.pBindings    = bindings.data();

    VkDescriptorSetLayout descriptor_set_layout;
    VK_CHECK(vkCreateDescriptorSetLayout(device,
                                         &descriptor_set_layout_info,
                                         nullptr,
                                         &descriptor_set_layout));

    const auto index = static_cast<u32>(descriptor_set_layouts.size());
    descriptor_set_layouts.insert({index, descriptor_set_layout});

    return static_cast<u32>(index);
}

const u32 VulkanDevice::create_descriptor_set(const resources::DescriptorSetDescriptor& descriptor)
{
    auto iterator = descriptor_set_layouts.find(descriptor.layout_id);
    if (iterator == descriptor_set_layouts.end())
    {
        PFATAL("Failed to find a valid descriptor set layout.");
    }

    auto layout = iterator->second;

    VkDescriptorSetAllocateInfo info = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
    info.descriptorPool              = descriptor_pool;
    info.descriptorSetCount          = 1;
    info.pSetLayouts                 = &layout;

    VkDescriptorSet descriptor_set;
    VK_CHECK(vkAllocateDescriptorSets(device, &info, &descriptor_set));

    const auto buffer = access_buffer(descriptor.buffer);
    ASSERT(buffer != nullptr);

    VkDescriptorBufferInfo buffer_info = {};
    buffer_info.buffer                 = buffer->buffer;
    buffer_info.range                  = VK_WHOLE_SIZE;
    buffer_info.offset                 = 0;

    VkWriteDescriptorSet write = {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
    write.descriptorCount      = 1;
    write.descriptorType       = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    write.dstBinding           = descriptor.binding;
    write.pBufferInfo          = &buffer_info;
    write.dstSet               = descriptor_set;

    vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);

    const auto index = static_cast<u32>(descriptor_sets.size());

    descriptor_sets.insert({index, descriptor_set});

    return index;
}
void VulkanDevice::begin_frame()
{
    VK_CHECK(vkWaitForFences(device, 1, &render_fences[current_frame], VK_TRUE, UINT64_MAX));
}

void VulkanDevice::create_pipeline(const resources::PipelineDescriptor& descriptor)
{
    ASSERT(VulkanPipeline::build_pipeline(device, &descriptor, render_pass) == true);
}

void VulkanDevice::end_frame()
{
    // Acquire swapchain image.
    auto ok = vkAcquireNextImageKHR(device,
                                    swapchain.swapchain,
                                    UINT64_MAX,
                                    present_semaphores[current_frame],
                                    nullptr,
                                    &swapchain_index);

    // Recreate if not valid.
    if (ok == VK_ERROR_OUT_OF_DATE_KHR)
    {
        recreate_swapchain();
        return;
    }
    else if (ok != VK_SUCCESS && ok != VK_SUBOPTIMAL_KHR)
    {
        PFATAL("Failed to acquire swapchain image!");
    }

    // Reset fence only when we know we are submitting work to the gpu.
    VK_CHECK(vkResetFences(device, 1, &render_fences[current_frame]));

    // Record commands.
    vkCmdEndRenderPass(command_buffers[current_frame].cmd);

    VK_CHECK(vkEndCommandBuffer(command_buffers[current_frame].cmd));

    // Submit commands
    VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo submit         = vkinit::submit_info(&command_buffers[current_frame].cmd);
    submit.pWaitDstStageMask    = &waitStage;
    submit.waitSemaphoreCount   = 1;
    submit.pWaitSemaphores      = &present_semaphores[current_frame];
    submit.signalSemaphoreCount = 1;
    submit.pSignalSemaphores    = &render_semaphores[current_frame];

    VK_CHECK(vkQueueSubmit(graphics_queue, 1, &submit, render_fences[current_frame]));

    // Present image.
    VkPresentInfoKHR present_info   = vkinit::present_info();
    present_info.swapchainCount     = 1;
    present_info.pSwapchains        = &swapchain.swapchain;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores    = &render_semaphores[current_frame];
    present_info.pImageIndices      = &swapchain_index;

    ok = vkQueuePresentKHR(present_queue, &present_info);
    if (ok == VK_ERROR_OUT_OF_DATE_KHR || ok == VK_SUBOPTIMAL_KHR)
    {
        recreate_swapchain();
    }
    else if (ok != VK_SUCCESS)
    {
        PFATAL("Failed to present swapchain image!");
    }

    current_frame = (current_frame + 1) % MAX_SWAPCHAIN_IMAGES;
}

resources::CommandBuffer* VulkanDevice::get_command_buffer(bool begin)
{
    if (begin)
    {
        VkCommandBufferBeginInfo cmd_begin_info = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
        cmd_begin_info.flags                    = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        vkBeginCommandBuffer(command_buffers[current_frame].cmd, &cmd_begin_info);
    }

    return &command_buffers[current_frame];
}

void* VulkanDevice::map_buffer(const resources::BufferHandle handle, u32 size)
{
    ASSERT(handle.id != INVALID_ID);
    ASSERT(size > 0);

    auto buffer = static_cast<VulkanBuffer*>(buffers.access_resource(handle.id));

    ASSERT(buffer != nullptr);

    void* data;
    vkMapMemory(device, buffer->memory, 0, size, 0, &data);

    return data;
}

void VulkanDevice::unmap_buffer(const resources::BufferHandle buffer_handle)
{
    ASSERT(buffer_handle.id != INVALID_ID);

    const auto buffer = access_buffer(buffer_handle);
    ASSERT(buffer != nullptr);

    vkUnmapMemory(device, buffer->memory);
}

void VulkanDevice::copy_buffer(const resources::BufferHandle src_buffer_handle,
                               const resources::BufferHandle dst_buffer_handle,
                               const u32                     size,
                               const u32                     src_offset,
                               const u32                     dst_offset)
{
    ASSERT(src_buffer_handle.id != dst_buffer_handle.id);
    // Get buffers
    // TODO simplify this.

    const auto src_buffer = access_buffer(src_buffer_handle);
    const auto dst_buffer = access_buffer(dst_buffer_handle);
    if (src_buffer == nullptr || dst_buffer == nullptr)
    {
        // TODO #include Windows.h gets in conflict with PERROR(). Find a way to user own logger.
        //PERROR("Trying to copy invalid buffers.");
        return;
    }

    // Copy logic
    VkCommandBufferAllocateInfo cmd_alloc_info = vkinit::command_buffer_allocate_info(command_pool);

    VkCommandBuffer copy_cmd;
    VK_CHECK(vkAllocateCommandBuffers(device, &cmd_alloc_info, &copy_cmd));

    VkCommandBufferBeginInfo begin_info = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    begin_info.flags                    = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(copy_cmd, &begin_info);

    VkBufferCopy region = {};
    region.size         = size;
    region.dstOffset    = dst_offset;
    region.srcOffset    = src_offset;
    vkCmdCopyBuffer(copy_cmd, src_buffer->buffer, dst_buffer->buffer, 1, &region);

    vkEndCommandBuffer(copy_cmd);

    VkSubmitInfo submit_info       = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers    = &copy_cmd;

    VK_CHECK(vkQueueSubmit(graphics_queue, 1, &submit_info, VK_NULL_HANDLE));
    vkQueueWaitIdle(graphics_queue);

    vkFreeCommandBuffers(device, command_pool, 1, &copy_cmd);
}

void VulkanDevice::destroy_buffer(resources::BufferHandle handle)
{
    const auto buffer = access_buffer(handle);

    vkDestroyBuffer(device, buffer->buffer, nullptr);
    vkFreeMemory(device, buffer->memory, nullptr);

    buffers.remove_resource(handle.id);
}

void VulkanDevice::destroy_texture(resources::TextureHandle handle)
{
    const auto texture = access_texture(handle);

    vkDestroySampler(device, texture->sampler, nullptr);
    vkDestroyImageView(device, texture->image_view, nullptr);
    vkDestroyImage(device, texture->image, nullptr);
    vkFreeMemory(device, texture->memory, nullptr);
}

bool VulkanDevice::create_instance()
{
    VkApplicationInfo app_info{VK_STRUCTURE_TYPE_APPLICATION_INFO};
    app_info.apiVersion         = VK_API_VERSION_1_2;
    app_info.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
    app_info.engineVersion      = VK_MAKE_VERSION(0, 1, 0);
    app_info.pApplicationName   = "SogasEngine";
    app_info.pEngineName        = "PinutEngine";

    VkInstanceCreateInfo instance_info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    instance_info.pApplicationInfo     = &app_info;

#ifdef _DEBUG
    if (!check_validation_layer_support(get_available_instance_layers()))
    {
        throw std::runtime_error("Required validation layers not supported.");
    }

    PDEBUG("Required validation layers found.");
    instance_info.enabledLayerCount   = static_cast<u32>(validation_layers.size());
    instance_info.ppEnabledLayerNames = validation_layers.data();

    for (const auto required_layer : validation_layers)
    {
        PDEBUG("\t%s", required_layer);
    }
    PDEBUG("");
#else
    instance_info.enabledLayerCount   = 0;
    instance_info.ppEnabledLayerNames = nullptr;
#endif

    if (!check_required_extensions_available(get_available_instance_extensions()))
    {
        throw std::runtime_error("Required instance extensions not supported.");
    }

#ifdef _DEBUG
    PDEBUG("Required instance extensions found.");
    for (const auto extension : required_instance_extensions)
    {
        PDEBUG("\t%s", extension);
    }
    PDEBUG("");
#endif

    instance_info.enabledExtensionCount   = static_cast<u32>(required_instance_extensions.size());
    instance_info.ppEnabledExtensionNames = required_instance_extensions.data();

    auto ok = vkCreateInstance(&instance_info, nullptr, &vulkan_instance);
    VK_CHECK(ok);
    return true;
}

VulkanBuffer* VulkanDevice::access_buffer(resources::BufferHandle handle)
{
    return static_cast<VulkanBuffer*>(buffers.access_resource(handle.id));
}

VulkanTexture* VulkanDevice::access_texture(resources::TextureHandle handle)
{
    return static_cast<VulkanTexture*>(textures.access_resource(handle.id));
}

#ifdef _DEBUG
void VulkanDevice::setup_debug_messenger()
{
    if (!enable_validation_layers)
        return;

    VkDebugUtilsMessengerCreateInfoEXT debug_messenger_create_info = {
      VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
    debug_messenger_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debug_messenger_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                              VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
                                              VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    debug_messenger_create_info.pfnUserCallback = debug_callback;
    debug_messenger_create_info.pUserData       = nullptr;

    if (create_debug_utils_messengerEXT(vulkan_instance,
                                        &debug_messenger_create_info,
                                        nullptr,
                                        &debug_messenger) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to setup debug messenger!");
    }
}
#endif

void VulkanDevice::pick_physical_device()
{
    u32 physical_device_count = 0;
    VK_CHECK(vkEnumeratePhysicalDevices(vulkan_instance, &physical_device_count, nullptr));

    if (physical_device_count == 0)
    {
        throw std::runtime_error("No available physical device.");
    }

    std::vector<VkPhysicalDevice> available_physical_devices(physical_device_count);
    VK_CHECK(vkEnumeratePhysicalDevices(vulkan_instance,
                                        &physical_device_count,
                                        available_physical_devices.data()));

#ifdef _DEBUG
    PDEBUG("Available physical devices: ");
    for (const auto& gpu : available_physical_devices)
    {
        vkGetPhysicalDeviceProperties(gpu, &physical_device_properties);
        PDEBUG("\tDevice name: %s\tVendor ID: %d",
               physical_device_properties.deviceName,
               physical_device_properties.vendorID);
    }
#endif

    // TODO Make a proper physical device selector. Is device suitable?
    // Right now just picking the first physical device available.
    ASSERT(available_physical_devices.at(0) != VK_NULL_HANDLE);
    physical_device = available_physical_devices.at(0);
    vkGetPhysicalDeviceProperties(physical_device, &physical_device_properties);

    if (!check_device_extension_support(physical_device))
    {
        throw std::runtime_error("Selected device does not support required extensions.");
    }
}

std::vector<VkDeviceQueueCreateInfo> VulkanDevice::get_queues()
{
    u32 queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, nullptr);

    queue_family_properties.resize(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device,
                                             &queue_family_count,
                                             queue_family_properties.data());

    u32 i = 0;
    for (const auto& queue_family : queue_family_properties)
    {
        if (queue_family.queueCount > 0 &&
            queue_family.queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT))
        {
            graphics_family = i;
        }

        VkBool32 present_support = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, vulkan_surface, &present_support);

        if (present_support)
        {
            present_family = i;
        }

        if (graphics_family == present_family && graphics_family != VK_QUEUE_FAMILY_IGNORED &&
            present_family != VK_QUEUE_FAMILY_IGNORED)
        {
            break;
        }

        ++i;
    }

    std::vector<VkDeviceQueueCreateInfo> queueue_create_infos;
    std::set<u32>                        unique_queue_families = {graphics_family, present_family};

    f32 queue_priority = 1.0f;
    for (u32 queue_family : unique_queue_families)
    {
        VkDeviceQueueCreateInfo create_info = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
        create_info.queueCount              = 1;
        create_info.pQueuePriorities        = &queue_priority;
        create_info.queueFamilyIndex        = queue_family;

        queueue_create_infos.push_back(create_info);
    }

    return queueue_create_infos;
}

void VulkanDevice::create_device()
{
    pick_physical_device();

    auto queue_create_infos = get_queues();

    VkPhysicalDeviceDescriptorIndexingFeatures indexing_features{
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES,
      nullptr};

    VkPhysicalDeviceFeatures2 physical_features2 = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
                                                    &indexing_features};
    vkGetPhysicalDeviceFeatures2(physical_device, &physical_features2);

    bool bIs_bindless_supported =
      indexing_features.descriptorBindingPartiallyBound && indexing_features.runtimeDescriptorArray;

    VkDeviceCreateInfo device_create_info    = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
    device_create_info.queueCreateInfoCount  = static_cast<u32>(queue_create_infos.size());
    device_create_info.pQueueCreateInfos     = queue_create_infos.data();
    device_create_info.enabledExtensionCount = static_cast<u32>(required_device_extensions.size());
    device_create_info.ppEnabledExtensionNames = required_device_extensions.data();

    if (bIs_bindless_supported)
    {
        device_create_info.pNext = &physical_features2;
    }

    if (enable_validation_layers)
    {
        device_create_info.enabledLayerCount   = static_cast<u32>(validation_layers.size());
        device_create_info.ppEnabledLayerNames = validation_layers.data();
    }
    else
    {
        device_create_info.enabledLayerCount   = 0;
        device_create_info.ppEnabledLayerNames = nullptr;
    }

    VkResult ok = vkCreateDevice(physical_device, &device_create_info, nullptr, &device);
    VK_CHECK(ok);

    vkGetDeviceQueue(device, graphics_family, 0, &graphics_queue);
    vkGetDeviceQueue(device, present_family, 0, &present_queue);
}

void VulkanDevice::create_surface(void* window)
{
    if (window == nullptr)
    {
        throw std::runtime_error("Received a nullptr window.");
    }

#ifdef WIN32
    VkWin32SurfaceCreateInfoKHR surfaceInfo = {VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR};
    surfaceInfo.hwnd                        = (HWND)window;
    surfaceInfo.hinstance                   = 0;

    if (vkCreateWin32SurfaceKHR(vulkan_instance, &surfaceInfo, nullptr, &vulkan_surface) !=
        VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create windows surface.");
    }
#endif

    PDEBUG("Window surface created!");
}

u32 VulkanDevice::find_memory_type(const u32                   type_filter,
                                   const VkMemoryPropertyFlags property_flags)
{
    for (u32 i = 0; i < physical_device_memory_properties.memoryTypeCount; i++)
    {
        if (type_filter & (1 << i) &&
            (physical_device_memory_properties.memoryTypes[i].propertyFlags & property_flags))
        {
            return i;
        }
    }

    throw std::runtime_error("Failed to find suitable memory type!");
}

void VulkanDevice::create_swapchain()
{
    VulkanSwapchainBuilder swapchain_builder{device,
                                             physical_device,
                                             vulkan_surface,
                                             graphics_family,
                                             present_family};

    swapchain = swapchain_builder.use_default_format()
                  .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
                  .set_desired_extent(extent.width, extent.height)
                  .build();

    swapchain_images      = swapchain.get_images();
    swapchain_image_views = swapchain.get_image_views();
}

void VulkanDevice::destroy_swapchain()
{
    for (auto& framebuffer : framebuffers)
    {
        vkDestroyFramebuffer(device, framebuffer, nullptr);
    }

    for (auto image_view : swapchain_image_views)
    {
        vkDestroyImageView(device, image_view, nullptr);
    }

    // No need to destroy swapchain images here since they were not created by us.
    vkDestroySwapchainKHR(device, swapchain.swapchain, nullptr);
}

void VulkanDevice::recreate_swapchain()
{
    vkDeviceWaitIdle(device);

    if (!minimized)
    {
        // Window closing.
        if (new_extent.width == 0 && new_extent.height == 0)
        {
            return;
        }

        destroy_swapchain();
        vkDestroySurfaceKHR(vulkan_instance, vulkan_surface, nullptr);
    }

    extent = new_extent;

    if (extent.width == 0 || extent.height == 0)
    {
        minimized = true;
        return;
    }

    minimized = false;

    create_surface(window_handle);

    create_swapchain();

    VkFramebufferCreateInfo framebuffer_info = {VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
    framebuffer_info.renderPass              = render_pass;
    framebuffer_info.width                   = extent.width;
    framebuffer_info.height                  = extent.height;
    framebuffer_info.attachmentCount         = 1;
    framebuffer_info.layers                  = 1;

    const u32 swapchain_images_size = static_cast<u32>(swapchain_images.size());

    for (u32 i = 0; i < swapchain_images_size; ++i)
    {
        framebuffer_info.pAttachments = &swapchain_image_views.at(i);

        VK_CHECK(vkCreateFramebuffer(device, &framebuffer_info, nullptr, &framebuffers[i]));
    }
}

void VulkanDevice::create_vulkan_buffer(const u32             size,
                                        VkBufferUsageFlags    usage_flags,
                                        VkMemoryPropertyFlags memory_property_flags,
                                        VulkanBuffer*         buffer)
{
    auto info = vkinit::buffer_info(size, usage_flags);

    VK_CHECK(vkCreateBuffer(device, &info, nullptr, &buffer->buffer));

    VkMemoryRequirements memory_requirements;
    vkGetBufferMemoryRequirements(device, buffer->buffer, &memory_requirements);

    VkMemoryAllocateInfo allocate_info = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    allocate_info.allocationSize       = memory_requirements.size;
    allocate_info.memoryTypeIndex =
      find_memory_type(memory_requirements.memoryTypeBits, memory_property_flags);

    VK_CHECK(vkAllocateMemory(device, &allocate_info, nullptr, &buffer->memory));

    vkBindBufferMemory(device, buffer->buffer, buffer->memory, 0);
}
} // namespace vulkan
} // namespace pinut
