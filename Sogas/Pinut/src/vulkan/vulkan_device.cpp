#include "pch.hpp"

#include <vulkan/utils/vulkan_initializers.h>
#include <vulkan/utils/vulkan_render_pass.h>
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

static void transition_image_layout(VkCommandBuffer cmd,
                                    VkImage         image,
                                    VkFormat /*format*/,
                                    VkImageLayout old_layout,
                                    VkImageLayout new_layout)
{
    // TODO Format parameter will be used to check if depth image has stencil.

    VkImageMemoryBarrier barrier{VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
    barrier.image                           = image;
    barrier.oldLayout                       = old_layout;
    barrier.newLayout                       = new_layout;
    barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.baseMipLevel   = 0;
    barrier.subresourceRange.layerCount     = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.levelCount     = 1;
    barrier.srcAccessMask                   = 0;
    barrier.dstAccessMask                   = VK_ACCESS_TRANSFER_WRITE_BIT;

    VkPipelineStageFlagBits source_stage      = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
    VkPipelineStageFlagBits destination_stage = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;

    if (new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;

        // TODO check for stencil
    }
    else
    {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }

    if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED &&
        new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        source_stage      = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
             new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        source_stage      = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED &&
             new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        source_stage      = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destination_stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    }
    else
    {
        throw std::invalid_argument("unsupported layout transition!");
    }

    vkCmdPipelineBarrier(cmd,
                         source_stage,
                         destination_stage,
                         0,
                         0,
                         nullptr,
                         0,
                         nullptr,
                         1,
                         &barrier);
}

static bool create_shader_module(VkDevice                             device,
                                 const pinut::resources::ShaderStage& shader_stage,
                                 VkShaderModule&                      shader_module)
{
    VkShaderModuleCreateInfo info = {VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
    info.codeSize                 = shader_stage.code.size() * sizeof(u32);
    info.pCode                    = shader_stage.code.data();

    if (vkCreateShaderModule(device, &info, nullptr, &shader_module) != VK_SUCCESS)
    {
        PFATAL("Failed to create shader module!");
        return false;
    }

    return true;
}

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

VulkanDevice::~VulkanDevice()
{
    shutdown();
}

void VulkanDevice::init(const DeviceDescriptor& descriptor)
{
    window_handle = descriptor.window;
    extent        = {descriptor.width, descriptor.height};

    deletion_queue.reserve(32);

    PDEBUG("GPU Device init.");
    create_instance();
    create_surface(window_handle);
    create_device();
    create_swapchain();

    // Create command pool
    VkCommandPoolCreateInfo cmd_pool_info =
      vkinit::command_pool_create_info(graphics_family,
                                       VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    VK_CHECK(vkCreateCommandPool(device, &cmd_pool_info, nullptr, &command_pool));

    buffers.init(DEFAULT_RESOURCES_COUNT, sizeof(VulkanBuffer));
    textures.init(DEFAULT_RESOURCES_COUNT, sizeof(VulkanTexture));
    descriptor_sets.init(DEFAULT_RESOURCES_COUNT, sizeof(VulkanDescriptorSet));
    descriptor_set_layouts.init(DEFAULT_RESOURCES_COUNT, sizeof(VulkanDescriptorSetLayout));

    depth_texture = create_texture({nullptr,
                                    descriptor.width,
                                    descriptor.height,
                                    1,
                                    resources::TextureType::TEXTURE_2D,
                                    resources::TextureFormat::D32_SFLOAT,
                                    1,
                                    1,
                                    "DepthTexture"});

    auto texture = access_texture(depth_texture.id);
    auto cmd     = begin_single_use_command_buffer();
    transition_image_layout(cmd,
                            texture->image,
                            VK_FORMAT_D32_SFLOAT,
                            VK_IMAGE_LAYOUT_UNDEFINED,
                            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    end_single_use_command_buffer(cmd);

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

    VkAttachmentDescription depth_attachment = {};
    depth_attachment.format                  = VK_FORMAT_D32_SFLOAT;
    depth_attachment.samples                 = VK_SAMPLE_COUNT_1_BIT;
    depth_attachment.loadOp                  = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment.storeOp                 = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.stencilLoadOp           = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depth_attachment.stencilStoreOp          = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.initialLayout           = VK_IMAGE_LAYOUT_UNDEFINED;
    depth_attachment.finalLayout             = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference color_attachment_reference = {};
    color_attachment_reference.attachment            = 0;
    color_attachment_reference.layout                = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depth_attachment_reference = {};
    depth_attachment_reference.layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    depth_attachment_reference.attachment = 1;

    VkSubpassDescription subpass    = {};
    subpass.colorAttachmentCount    = 1;
    subpass.pColorAttachments       = &color_attachment_reference;
    subpass.pDepthStencilAttachment = &depth_attachment_reference;
    subpass.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass          = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass          = 0;
    dependency.srcStageMask        = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask       = 0;
    dependency.dstStageMask        = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask       = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkSubpassDependency depth_dependency = {};
    depth_dependency.srcSubpass          = VK_SUBPASS_EXTERNAL;
    depth_dependency.dstSubpass          = 0;
    depth_dependency.srcStageMask =
      VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    depth_dependency.srcAccessMask = 0;
    depth_dependency.dstStageMask =
      VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    depth_dependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    VkAttachmentDescription attachment_descriptions[] = {color_attachment, depth_attachment};
    VkSubpassDependency     dependencies[]            = {dependency, depth_dependency};

    VkRenderPassCreateInfo render_pass_info = {VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
    render_pass_info.attachmentCount        = 2;
    render_pass_info.pAttachments           = attachment_descriptions;
    render_pass_info.subpassCount           = 1;
    render_pass_info.pSubpasses             = &subpass;
    render_pass_info.dependencyCount        = 2;
    render_pass_info.pDependencies          = dependencies;

    VK_CHECK(vkCreateRenderPass(device, &render_pass_info, nullptr, &render_pass));

    VulkanRenderPass vulkan_renderpass = {};
    vulkan_renderpass.type             = resources::RenderPassType::SWAPCHAIN;
    vulkan_renderpass.handle           = render_pass;
    vulkan_renderpass.width            = static_cast<u16>(extent.width);
    vulkan_renderpass.height           = static_cast<u16>(extent.height);

    VulkanDevice::render_passes.insert({"Swapchain_renderpass", vulkan_renderpass});

    VkFramebufferCreateInfo framebuffer_info = vkinit::framebuffer_create_info(render_pass, extent);

    const u32 swapchain_images_size = static_cast<u32>(swapchain_images.size());

    const auto vulkan_depth_texture = access_texture(depth_texture.id);

    for (u32 i = 0; i < swapchain_images_size; ++i)
    {
        VkImageView attachments[2];
        attachments[0] = swapchain_image_views.at(i);
        attachments[1] = vulkan_depth_texture->image_view;

        framebuffer_info.attachmentCount = 2;
        framebuffer_info.pAttachments    = attachments;

        VK_CHECK(vkCreateFramebuffer(device, &framebuffer_info, nullptr, &framebuffers[i]));
    }

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
    VkDescriptorPoolSize pool_sizes[] = {
      {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, MAX_SWAPCHAIN_IMAGES},
      {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, MAX_SWAPCHAIN_IMAGES}};

    VkDescriptorPoolCreateInfo pool_info = {VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
    pool_info.poolSizeCount              = 2;
    pool_info.pPoolSizes                 = pool_sizes;
    pool_info.maxSets                    = static_cast<u32>(MAX_SWAPCHAIN_IMAGES);
    pool_info.flags                      = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

    VK_CHECK(vkCreateDescriptorPool(device, &pool_info, nullptr, &descriptor_pool));
}

void VulkanDevice::shutdown()
{
    VK_CHECK(vkWaitForFences(device, MAX_SWAPCHAIN_IMAGES, render_fences, VK_TRUE, UINT64_MAX));
    VK_CHECK(vkDeviceWaitIdle(device));

    destroy_texture_immediate(depth_texture.id);

    destroy_pending_resources();
    deletion_queue.clear();

    buffers.shutdown();
    textures.shutdown();
    descriptor_sets.shutdown();
    descriptor_set_layouts.shutdown();

    vkDestroyDescriptorPool(device, descriptor_pool, nullptr);

    // Clear shaders
    for (auto& shader : shaders)
    {
        for (auto& module : shader.second)
        {
            vkDestroyShaderModule(device, module.module, nullptr);
        }
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

    auto buffer = access_buffer(handle.id);

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

    VulkanTexture* texture = access_texture(handle.id);
    texture->format        = get_texture_format(descriptor.format);

    VkImageType texture_type = get_texture_type(descriptor.type);

    VkImageCreateInfo info = {VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
    info.mipLevels         = descriptor.mip_levels;
    info.imageType         = texture_type;
    info.extent            = {descriptor.width, descriptor.height, 1};
    info.arrayLayers       = 1;
    info.format            = texture->format;
    info.tiling            = VK_IMAGE_TILING_OPTIMAL;
    info.initialLayout     = VK_IMAGE_LAYOUT_UNDEFINED;
    info.sharingMode       = VK_SHARING_MODE_EXCLUSIVE;
    info.samples           = VK_SAMPLE_COUNT_1_BIT;

    bool has_depth = has_depth_or_stencil(texture->format);
    if (has_depth)
    {
        info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    }
    else
    {
        info.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    }

    auto ok = vkCreateImage(device, &info, nullptr, &texture->image);
    VK_CHECK(ok);

    VkMemoryRequirements memory_requirements;
    vkGetImageMemoryRequirements(device, texture->image, &memory_requirements);

    VkMemoryAllocateInfo allocate_info = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    allocate_info.allocationSize       = memory_requirements.size;
    allocate_info.memoryTypeIndex =
      find_memory_type(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VK_CHECK(vkAllocateMemory(device, &allocate_info, nullptr, &texture->memory));

    vkBindImageMemory(device, texture->image, texture->memory, 0);

    if (descriptor.data != nullptr)
    {
        // TODO channels? hardcoded to 4.
        const u32 texture_size = descriptor.width * descriptor.height * 4;
        auto      staging_buffer_handle =
          create_buffer({texture_size, resources::BufferType::STAGING, descriptor.data});

        copy_buffer_to_image(staging_buffer_handle, handle, descriptor.width, descriptor.height);

        destroy_buffer(staging_buffer_handle);
    }

    VkImageViewCreateInfo image_view_info = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    image_view_info.image                 = texture->image;
    image_view_info.viewType              = VK_IMAGE_VIEW_TYPE_2D;
    image_view_info.format                = texture->format;
    image_view_info.subresourceRange.aspectMask =
      has_depth ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
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

resources::DescriptorSetLayoutHandle VulkanDevice::create_descriptor_set_layout(
  const resources::DescriptorSetLayoutDescriptor& descriptor)
{
    ASSERT(descriptor.binding_count > 0);

    resources::DescriptorSetLayoutHandle layout_handle = {descriptor_set_layouts.get_resource()};
    if (layout_handle.id == resources::invalid_descriptor_set_layout.id)
    {
        return layout_handle;
    }

    auto descriptor_set_layout = access_descriptor_set_layout(layout_handle.id);
    ASSERT(descriptor_set_layout != nullptr);

    descriptor_set_layout->bindings = (resources::DescriptorSetBindingDescriptor*)malloc(
      sizeof(resources::DescriptorSetBindingDescriptor) * descriptor.binding_count);

    if (!descriptor_set_layout->bindings)
    {
        throw std::runtime_error("Not enough memory for bindings.");
    }

    memset(descriptor_set_layout->bindings,
           {},
           sizeof(resources::DescriptorSetBindingDescriptor) * descriptor.binding_count);

    std::vector<VkDescriptorSetLayoutBinding> bindings(descriptor.binding_count);
    for (u32 i = 0; i < descriptor.binding_count; ++i)
    {
        auto& in_binding = descriptor.bindings[i];

        descriptor_set_layout->bindings[i] = in_binding;
        descriptor_set_layout->bindings_count++;

        VkDescriptorSetLayoutBinding binding = {};
        binding.binding                      = in_binding.binding;
        binding.descriptorCount              = in_binding.count;
        binding.descriptorType               = get_descriptor_type(in_binding.descriptor_type);
        binding.stageFlags                   = get_shader_stage_flag(in_binding.shader_stage);

        bindings.at(i) = binding;
    }

    VkDescriptorSetLayoutCreateInfo descriptor_set_layout_info = {
      VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
    descriptor_set_layout_info.bindingCount = static_cast<u32>(bindings.size());
    descriptor_set_layout_info.pBindings    = bindings.data();

    VK_CHECK(vkCreateDescriptorSetLayout(device,
                                         &descriptor_set_layout_info,
                                         nullptr,
                                         &descriptor_set_layout->layout));

    return layout_handle;
}

resources::DescriptorSetHandle VulkanDevice::create_descriptor_set(
  const resources::DescriptorSetDescriptor& descriptor)
{
    const resources::DescriptorSetHandle handle{descriptor_sets.get_resource()};
    if (handle.id == resources::invalid_descriptor_set.id)
    {
        return handle;
    }

    const auto descriptor_set = access_descriptor_set(handle.id);
    const auto layout         = access_descriptor_set_layout(descriptor.layout_handle.id);

    VkDescriptorSetAllocateInfo info = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
    info.descriptorPool              = descriptor_pool;
    info.descriptorSetCount          = 1;
    info.pSetLayouts                 = &layout->layout;

    VK_CHECK(vkAllocateDescriptorSets(device, &info, &descriptor_set->descriptor_set));

    // TODO Why 8??
    VkWriteDescriptorSet   write[8]       = {};
    VkDescriptorImageInfo  image_info[8]  = {};
    VkDescriptorBufferInfo buffer_info[8] = {};

    for (u32 i = 0; i < descriptor.resources_used; ++i)
    {
        auto binding     = &layout->bindings[i];
        binding->binding = descriptor.bindings[i];

        write[i]                 = {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
        write[i].descriptorCount = binding->count;
        write[i].dstSet          = descriptor_set->descriptor_set;
        write[i].dstBinding      = binding->binding;
        write[i].dstArrayElement = 0;

        switch (binding->descriptor_type)
        {
            case resources::DescriptorType::COMBINED_IMAGE_SAMPLER:
            {
                resources::TextureHandle texture_handle{descriptor.resources[i]};
                const auto               texture = access_texture(texture_handle.id);

                image_info[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                image_info[i].imageView   = texture->image_view;
                image_info[i].sampler     = texture->sampler;

                write[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                write[i].pImageInfo     = &image_info[i];
                break;
            }
            case resources::DescriptorType::UNIFORM:
            {
                resources::BufferHandle buffer_handle{descriptor.resources[i]};
                const auto              buffer = access_buffer(buffer_handle.id);
                ASSERT(buffer != nullptr);

                buffer_info[i].buffer = buffer->buffer;
                buffer_info[i].range  = VK_WHOLE_SIZE;
                buffer_info[i].offset = 0;

                write[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                write[i].pBufferInfo    = &buffer_info[i];
                break;
            }
            default:
                ASSERT(false);
                break;
        }
    }

    vkUpdateDescriptorSets(device, descriptor.resources_used, write, 0, nullptr);

    return handle;
}
void VulkanDevice::begin_frame()
{
    VK_CHECK(vkWaitForFences(device, 1, &render_fences[current_frame], VK_TRUE, UINT64_MAX));
}

void VulkanDevice::create_pipeline(const resources::PipelineDescriptor& descriptor)
{
    VulkanShaderState pipeline_shader_stages_info;
    if (VulkanDevice::shaders.contains(descriptor.shader_state.name))
    {
        pipeline_shader_stages_info = VulkanDevice::shaders.at(descriptor.shader_state.name);
    }
    else
    {
        const u32 shader_stages_count =
          static_cast<u32>(descriptor.shader_state.number_used_stages);
        pipeline_shader_stages_info.resize(shader_stages_count);

        u32 index = 0;
        for (u32 i = 0; i < static_cast<u32>(resources::ShaderStageType::MAX_SHADER_TYPE); ++i)
        {
            if (descriptor.shader_state.shader_stages[i].code.empty())
            {
                continue;
            }

            const auto&    shader_stage = descriptor.shader_state.shader_stages[i];
            VkShaderModule module;
            if (!create_shader_module(device, shader_stage, module))
            {
                PFATAL("Failed to create shader module!");
                throw std::runtime_error("Failed to create shader module");
            }

            VkPipelineShaderStageCreateInfo info = {
              VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
            info.stage  = shader_stage.type == resources::ShaderStageType::VERTEX ?
                            VK_SHADER_STAGE_VERTEX_BIT :
                            VK_SHADER_STAGE_FRAGMENT_BIT;
            info.module = module;
            info.pName  = "main";

            pipeline_shader_stages_info.at(index) = info;
            index++;
        }

        VulkanDevice::shaders.insert({descriptor.shader_state.name, pipeline_shader_stages_info});
    }

    VkPipelineRasterizationStateCreateInfo rasterization_info = {
      VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
    rasterization_info.cullMode  = vulkan::get_cull_mode_flag(descriptor.rasterization.cull_mode);
    rasterization_info.frontFace = vulkan::get_front_face_flag(descriptor.rasterization.front_face);
    rasterization_info.polygonMode = vulkan::get_cull_mode_flag(descriptor.rasterization.fill_mode);
    rasterization_info.lineWidth   = descriptor.rasterization.line_width;
    rasterization_info.depthClampEnable        = VK_FALSE;
    rasterization_info.rasterizerDiscardEnable = VK_FALSE;
    rasterization_info.depthBiasEnable         = VK_FALSE;
    rasterization_info.depthBiasClamp          = 0.0f;
    rasterization_info.depthBiasConstantFactor = 0.0f;
    rasterization_info.depthBiasSlopeFactor    = 0.0f;

    VkPipelineInputAssemblyStateCreateInfo assembly_info = {
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
    assembly_info.topology               = get_topology(descriptor.topology);
    assembly_info.primitiveRestartEnable = VK_FALSE;

    VkPipelineVertexInputStateCreateInfo vertex_input_info = {
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};

    VkVertexInputAttributeDescription vertex_input_attribute_description[4] = {};
    if (descriptor.vertex_input.attribute_count > 0)
    {
        ASSERT(descriptor.vertex_input.attribute_count <= 4);

        for (u32 i = 0; i < descriptor.vertex_input.attribute_count; i++)
        {
            vertex_input_attribute_description[i].binding =
              descriptor.vertex_input.attributes[i].binding;
            vertex_input_attribute_description[i].location =
              descriptor.vertex_input.attributes[i].location;
            vertex_input_attribute_description[i].offset =
              descriptor.vertex_input.attributes[i].offset;
            vertex_input_attribute_description[i].format =
              get_vertex_input_format(descriptor.vertex_input.attributes[i].format_type);
        }

        vertex_input_info.vertexAttributeDescriptionCount = descriptor.vertex_input.attribute_count;
        vertex_input_info.pVertexAttributeDescriptions    = vertex_input_attribute_description;
    }
    else
    {
        vertex_input_info.vertexAttributeDescriptionCount = 0;
        vertex_input_info.pVertexAttributeDescriptions    = nullptr;
    }

    if (descriptor.vertex_input.stream_count > 0)
    {
        VkVertexInputBindingDescription input_binding_description[4];

        for (u32 i = 0; i < descriptor.vertex_input.stream_count; i++)
        {
            input_binding_description[0].binding = descriptor.vertex_input.streams[i].binding;
            input_binding_description[0].stride  = descriptor.vertex_input.streams[i].stride;
            input_binding_description[0].inputRate =
              get_vertex_input_rate(descriptor.vertex_input.streams[i].input_rate);
        }

        vertex_input_info.vertexBindingDescriptionCount = descriptor.vertex_input.stream_count;
        vertex_input_info.pVertexBindingDescriptions    = input_binding_description;
    }
    else
    {
        vertex_input_info.vertexBindingDescriptionCount = 0;
        vertex_input_info.pVertexBindingDescriptions    = nullptr;
    }

    VkPipelineDepthStencilStateCreateInfo depth_stencil_info = {
      VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
    depth_stencil_info.depthTestEnable       = VK_TRUE;
    depth_stencil_info.depthWriteEnable      = VK_TRUE;
    depth_stencil_info.depthCompareOp        = VK_COMPARE_OP_LESS;
    depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
    depth_stencil_info.stencilTestEnable     = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling_info = {
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
    multisampling_info.sampleShadingEnable   = VK_FALSE;
    multisampling_info.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
    multisampling_info.minSampleShading      = 1.0f;
    multisampling_info.pSampleMask           = nullptr;
    multisampling_info.alphaToCoverageEnable = VK_FALSE;
    multisampling_info.alphaToOneEnable      = VK_FALSE;

    VkPipelineColorBlendAttachmentState color_blend_attachment = {
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
    color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    color_blend_attachment.blendEnable = VK_FALSE;

    VkViewport viewport;
    viewport.x        = descriptor.viewport.viewport.x;
    viewport.y        = descriptor.viewport.viewport.y;
    viewport.width    = static_cast<float>(descriptor.viewport.viewport.width);
    viewport.height   = static_cast<float>(descriptor.viewport.viewport.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor;
    scissor.offset = {0, 0};
    scissor.extent = {descriptor.viewport.rect.w, descriptor.viewport.rect.h};

    VkPipelineViewportStateCreateInfo viewport_info = {
      VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
    viewport_info.viewportCount = 1;
    viewport_info.pViewports    = &viewport;
    viewport_info.scissorCount  = 1;
    viewport_info.pScissors     = &scissor;

    std::vector<VkDynamicState> dynamic_states = {VK_DYNAMIC_STATE_VIEWPORT,
                                                  VK_DYNAMIC_STATE_SCISSOR};

    VkPipelineDynamicStateCreateInfo dynamic_state_info = {
      VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
    dynamic_state_info.dynamicStateCount = static_cast<u32>(dynamic_states.size());
    dynamic_state_info.pDynamicStates    = dynamic_states.data();

    VkPipelineColorBlendStateCreateInfo color_blend_info = {
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
    color_blend_info.logicOpEnable   = VK_FALSE;
    color_blend_info.logicOp         = VK_LOGIC_OP_COPY;
    color_blend_info.attachmentCount = 1;
    color_blend_info.pAttachments    = &color_blend_attachment;

    VulkanPipeline pipeline;

    VkDescriptorSetLayout set_layouts[8];

    for (u32 i = 0; i < descriptor.layouts_count; ++i)
    {
        const auto layout = access_descriptor_set_layout(descriptor.descriptor_set_layouts[i].id);

        set_layouts[i] = layout->layout;
    }

    VkPipelineLayoutCreateInfo pipeline_layout_info = {
      VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
    pipeline_layout_info.setLayoutCount = descriptor.layouts_count;
    pipeline_layout_info.pSetLayouts    = set_layouts;

    VkPushConstantRange push_constants[8] = {};
    for (u32 i = 0; i < descriptor.push_constant_count; ++i)
    {
        push_constants[i].stageFlags = get_shader_stage_flag(descriptor.push_constants[i].stage);
        push_constants[i].size       = descriptor.push_constants[i].size;
        push_constants[i].offset     = descriptor.push_constants[i].offset;
    }

    pipeline_layout_info.pushConstantRangeCount = descriptor.push_constant_count;
    pipeline_layout_info.pPushConstantRanges    = push_constants;

    VK_CHECK(
      vkCreatePipelineLayout(device, &pipeline_layout_info, nullptr, &pipeline.pipeline_layout));

    VkGraphicsPipelineCreateInfo info = {VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
    info.stageCount                   = static_cast<u32>(pipeline_shader_stages_info.size());
    info.pStages                      = pipeline_shader_stages_info.data();
    info.pVertexInputState            = &vertex_input_info;
    info.pInputAssemblyState          = &assembly_info;
    info.pViewportState               = &viewport_info;
    info.pRasterizationState          = &rasterization_info;
    info.pDepthStencilState           = &depth_stencil_info;
    info.pMultisampleState            = &multisampling_info;
    info.pColorBlendState             = &color_blend_info;
    info.layout                       = pipeline.pipeline_layout;
    info.renderPass                   = render_pass;
    info.subpass                      = 0;
    info.pDynamicState                = &dynamic_state_info;
    info.basePipelineHandle           = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &info, nullptr, &pipeline.pipeline) !=
        VK_SUCCESS)
    {
        PFATAL("Failed to create pipeline.");
        throw std::runtime_error("Failed to create pipeline handle.");
    }

    VulkanDevice::pipelines.insert({descriptor.name, pipeline});
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

    destroy_pending_resources();

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

    const auto buffer = access_buffer(buffer_handle.id);
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

    const auto src_buffer = access_buffer(src_buffer_handle.id);
    const auto dst_buffer = access_buffer(dst_buffer_handle.id);
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

void VulkanDevice::copy_buffer_to_image(const resources::BufferHandle  buffer_handle,
                                        const resources::TextureHandle texture_handle,
                                        const u32                      width,
                                        const u32                      height)
{
    auto buffer  = access_buffer(buffer_handle.id);
    auto texture = access_texture(texture_handle.id);

    VkBufferImageCopy region = {};
    region.bufferOffset      = 0;
    region.bufferRowLength   = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel       = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount     = 1;

    region.imageOffset = {0, 0, 0};
    region.imageExtent = {width, height, 1};

    auto cmd = begin_single_use_command_buffer();

    transition_image_layout(cmd,
                            texture->image,
                            texture->format,
                            VK_IMAGE_LAYOUT_UNDEFINED,
                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    vkCmdCopyBufferToImage(cmd,
                           buffer->buffer,
                           texture->image,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           1,
                           &region);

    transition_image_layout(cmd,
                            texture->image,
                            texture->format,
                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    end_single_use_command_buffer(cmd);
}

void VulkanDevice::destroy_buffer(resources::BufferHandle handle)
{
    deletion_queue.push_back({resources::ResourceDestroyType::BUFFER, handle.id});
}

void VulkanDevice::destroy_texture(resources::TextureHandle handle)
{
    deletion_queue.push_back({resources::ResourceDestroyType::TEXTURE, handle.id});
}

void VulkanDevice::destroy_descriptor_set(resources::DescriptorSetHandle handle)
{
    deletion_queue.push_back({resources::ResourceDestroyType::DESCRIPTOR_SET, handle.id});
}

void VulkanDevice::destroy_descriptor_set_layout(resources::DescriptorSetLayoutHandle handle)
{
    deletion_queue.push_back({resources::ResourceDestroyType::DESCRIPTOR_SET_LAYOUT, handle.id});
}

void VulkanDevice::destroy_buffer_immediate(resources::ResourceHandle handle)
{
    const auto buffer = access_buffer(handle);

    vkDestroyBuffer(device, buffer->buffer, nullptr);
    vkFreeMemory(device, buffer->memory, nullptr);

    buffers.remove_resource(handle);
}

void VulkanDevice::destroy_texture_immediate(resources::ResourceHandle handle)
{
    const auto texture = access_texture(handle);

    vkDestroySampler(device, texture->sampler, nullptr);
    vkDestroyImageView(device, texture->image_view, nullptr);
    vkDestroyImage(device, texture->image, nullptr);
    vkFreeMemory(device, texture->memory, nullptr);

    textures.remove_resource(handle);
}

void VulkanDevice::destroy_descriptor_set_immediate(resources::ResourceHandle handle)
{
    auto descriptor_set = access_descriptor_set(handle);

    vkFreeDescriptorSets(device, descriptor_pool, 1, &descriptor_set->descriptor_set);

    descriptor_sets.remove_resource(handle);
}

void VulkanDevice::destroy_descriptor_set_layout_immediate(resources::ResourceHandle handle)
{
    auto layout = access_descriptor_set_layout(handle);

    vkDestroyDescriptorSetLayout(device, layout->layout, nullptr);

    descriptor_set_layouts.remove_resource(handle);
}

void VulkanDevice::destroy_pending_resources()
{
    if (deletion_queue.empty())
    {
        return;
    }

    for (i32 i = (i32)deletion_queue.size() - 1; i >= 0; --i)
    {
        const auto handle = deletion_queue.at(i).handle;
        switch (deletion_queue.at(i).type)
        {
            case resources::ResourceDestroyType::BUFFER:
                destroy_buffer_immediate(handle);
                break;
            case resources::ResourceDestroyType::TEXTURE:
                destroy_texture_immediate(handle);
                break;
            case resources::ResourceDestroyType::SHADER_STATE:
                break;
            case resources::ResourceDestroyType::RENDER_PASS:
                break;
            case resources::ResourceDestroyType::DESCRIPTOR_SET:
                destroy_descriptor_set_immediate(handle);
                break;
            case resources::ResourceDestroyType::DESCRIPTOR_SET_LAYOUT:
                destroy_descriptor_set_layout_immediate(handle);
                break;
        }

        deletion_queue.erase(deletion_queue.begin() + i);
    }

    ASSERT(deletion_queue.empty());
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

VulkanBuffer* VulkanDevice::access_buffer(resources::ResourceHandle handle)
{
    return static_cast<VulkanBuffer*>(buffers.access_resource(handle));
}

VulkanTexture* VulkanDevice::access_texture(resources::ResourceHandle handle)
{
    return static_cast<VulkanTexture*>(textures.access_resource(handle));
}

VulkanDescriptorSet* VulkanDevice::access_descriptor_set(resources::ResourceHandle handle)
{
    return static_cast<VulkanDescriptorSet*>(descriptor_sets.access_resource(handle));
}

VulkanDescriptorSetLayout* VulkanDevice::access_descriptor_set_layout(
  resources::ResourceHandle handle)
{
    return static_cast<VulkanDescriptorSetLayout*>(descriptor_set_layouts.access_resource(handle));
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

    std::vector<VkFormat> depth_format_candidates = {VK_FORMAT_D32_SFLOAT,
                                                     VK_FORMAT_D32_SFLOAT_S8_UINT,
                                                     VK_FORMAT_D24_UNORM_S8_UINT};

    //VkImageTiling        depth_image_tiling  = VK_IMAGE_TILING_OPTIMAL;
    VkFormatFeatureFlags depth_feature_flags = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;

    for (auto& format : depth_format_candidates)
    {
        VkFormatProperties format_properties;
        vkGetPhysicalDeviceFormatProperties(physical_device, format, &format_properties);

        if (format_properties.optimalTilingFeatures & depth_feature_flags)
        {
            depth_texture_format = format;
            break;
        }
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

    vkGetPhysicalDeviceMemoryProperties(physical_device, &physical_device_memory_properties);

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

VkCommandBuffer VulkanDevice::begin_single_use_command_buffer()
{
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

    return command_buffer;
}

void VulkanDevice::end_single_use_command_buffer(VkCommandBuffer cmd)
{
    vkEndCommandBuffer(cmd);

    VkSubmitInfo submit_info       = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers    = &cmd;
    vkQueueSubmit(graphics_queue, 1, &submit_info, VK_NULL_HANDLE);

    vkQueueWaitIdle(graphics_queue);
    vkFreeCommandBuffers(device, command_pool, 1, &cmd);
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
