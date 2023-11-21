#include <vulkan/utils/vulkan_swapchain_builder.h>
#include <vulkan/vulkan_debug.h>
#include <vulkan/vulkan_device.h>

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

    create_swapchain();

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

    VkFramebufferCreateInfo framebuffer_info = {VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
    framebuffer_info.renderPass              = render_pass;
    framebuffer_info.width                   = extent.width;
    framebuffer_info.height                  = extent.height;
    framebuffer_info.attachmentCount         = 1;
    framebuffer_info.layers                  = 1;

    const u32 swapchain_images_size = static_cast<u32>(swapchain_images.size());
    framebuffers                    = std::vector<VkFramebuffer>(swapchain_images_size);

    for (u32 i = 0; i < swapchain_images_size; ++i)
    {
        framebuffer_info.pAttachments = &swapchain_image_views.at(i);

        VK_CHECK(vkCreateFramebuffer(device, &framebuffer_info, nullptr, &framebuffers.at(i)));
    }

    // Create command pool
    VkCommandPoolCreateInfo cmd_pool_info = {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
    cmd_pool_info.queueFamilyIndex        = graphics_family;
    cmd_pool_info.flags =
      VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Allow reseting of individual cmd.

    VK_CHECK(vkCreateCommandPool(device, &cmd_pool_info, nullptr, &command_pool));

    // Allocate main command buffer.
    VkCommandBufferAllocateInfo cmd_alloc_info = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    cmd_alloc_info.commandBufferCount          = 1;
    cmd_alloc_info.commandPool                 = command_pool;
    cmd_alloc_info.level                       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    VK_CHECK(vkAllocateCommandBuffers(device, &cmd_alloc_info, &cmd));

    // Create semaphores and fence
    VkSemaphoreCreateInfo semaphore_info = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    VK_CHECK(vkCreateSemaphore(device, &semaphore_info, nullptr, &render_semaphore));
    VK_CHECK(vkCreateSemaphore(device, &semaphore_info, nullptr, &present_semaphore));

    VkFenceCreateInfo fence_info = {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    fence_info.flags             = VK_FENCE_CREATE_SIGNALED_BIT;
    VK_CHECK(vkCreateFence(device, &fence_info, nullptr, &render_fence));

    // Create shaders
    vertex_shader_module = vulkan_shader_loader.create_shader_module(
      device,
      "../../Sogas/Engine/data/shaders/bin/triangle.vert.spv");

    fragment_shader_module = vulkan_shader_loader.create_shader_module(
      device,
      "../../Sogas/Engine/data/shaders/bin/triangle.frag.spv");

    if (!vertex_shader_module || !fragment_shader_module)
    {
        PFATAL("Failed to create shaders!");
    }

    // TODO: Handle pipeline creation differently ...
    // Data should be given from engine, not hardcoded in renderer.

    // TODO: Add all necessary data to the pipeline builder to build the pipeline.

    VkPipelineShaderStageCreateInfo vertex_shader_stage_info = {
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
    vertex_shader_stage_info.stage  = VK_SHADER_STAGE_VERTEX_BIT;
    vertex_shader_stage_info.module = vertex_shader_module;
    vertex_shader_stage_info.pName  = "main";

    VkPipelineShaderStageCreateInfo fragment_shader_stage_info = {
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
    fragment_shader_stage_info.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragment_shader_stage_info.module = fragment_shader_module;
    fragment_shader_stage_info.pName  = "main";

    VkPipelineInputAssemblyStateCreateInfo assembly_info = {
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
    assembly_info.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    assembly_info.primitiveRestartEnable = VK_FALSE;

    VkPipelineVertexInputStateCreateInfo vertex_input_info = {
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};

    VkPipelineRasterizationStateCreateInfo raster_info = {
      VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
    raster_info.depthClampEnable        = VK_FALSE;
    raster_info.rasterizerDiscardEnable = VK_FALSE;

    raster_info.polygonMode = VK_POLYGON_MODE_FILL;
    raster_info.lineWidth   = 1.0f;
    raster_info.frontFace   = VK_FRONT_FACE_CLOCKWISE;

    raster_info.depthBiasEnable         = VK_FALSE;
    raster_info.depthBiasConstantFactor = 0.0f;
    raster_info.depthBiasClamp          = 0.0f;
    raster_info.depthBiasSlopeFactor    = 0.0f;

    VkPipelineMultisampleStateCreateInfo multisampling_info = {
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
    multisampling_info.sampleShadingEnable   = VK_FALSE;
    multisampling_info.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
    multisampling_info.minSampleShading      = 1.0f;
    multisampling_info.pSampleMask           = nullptr;
    multisampling_info.alphaToCoverageEnable = VK_FALSE;
    multisampling_info.alphaToOneEnable      = VK_FALSE;

    VkPipelineColorBlendAttachmentState color_blend_info = {
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
    color_blend_info.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    color_blend_info.blendEnable = VK_FALSE;

    VkViewport viewport;
    viewport.x        = 0;
    viewport.y        = 0;
    viewport.width    = static_cast<float>(extent.width);
    viewport.height   = static_cast<float>(extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor;
    scissor.offset = {0, 0};
    scissor.extent = extent;

    std::vector<VkDynamicState> dynamic_states = {VK_DYNAMIC_STATE_VIEWPORT,
                                                  VK_DYNAMIC_STATE_SCISSOR};

    VkPipelineDynamicStateCreateInfo dynamic_state_info = {
      VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
    dynamic_state_info.dynamicStateCount = static_cast<u32>(dynamic_states.size());
    dynamic_state_info.pDynamicStates    = dynamic_states.data();

    VkPipelineLayoutCreateInfo pipeline_layout_info = {
      VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
    VK_CHECK(
      vkCreatePipelineLayout(device, &pipeline_layout_info, nullptr, &triangle_pipeline_layout));

    pipeline_builder.shader_stages.push_back(vertex_shader_stage_info);
    pipeline_builder.shader_stages.push_back(fragment_shader_stage_info);
    pipeline_builder.vertex_input           = vertex_input_info;
    pipeline_builder.assembly_input         = assembly_info;
    pipeline_builder.raster                 = raster_info;
    pipeline_builder.multisampling          = multisampling_info;
    pipeline_builder.color_blend_attachment = color_blend_info;
    pipeline_builder.viewport               = viewport;
    pipeline_builder.scissor                = scissor;
    pipeline_builder.dynamic_state          = dynamic_state_info;
    pipeline_builder.pipeline_layout        = triangle_pipeline_layout;

    triangle_pipeline = pipeline_builder.build_pipeline(device, render_pass);
}

void VulkanDevice::shutdown()
{
    VK_CHECK(vkWaitForFences(device, 1, &render_fence, VK_TRUE, UINT64_MAX));
    VK_CHECK(vkDeviceWaitIdle(device));

    vkDestroyPipeline(device, triangle_pipeline, nullptr);
    vkDestroyPipelineLayout(device, triangle_pipeline_layout, nullptr);
    vkDestroyShaderModule(device, vertex_shader_module, nullptr);
    vkDestroyShaderModule(device, fragment_shader_module, nullptr);
    vkDestroyFence(device, render_fence, nullptr);
    vkDestroySemaphore(device, present_semaphore, nullptr);
    vkDestroySemaphore(device, render_semaphore, nullptr);
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

void VulkanDevice::update()
{
    VK_CHECK(vkWaitForFences(device, 1, &render_fence, VK_TRUE, UINT64_MAX));

    if (minimized)
    {
        return;
    }

    u32  swapchainIndex;
    auto ok = vkAcquireNextImageKHR(device,
                                    swapchain.swapchain,
                                    UINT64_MAX,
                                    present_semaphore,
                                    nullptr,
                                    &swapchainIndex);

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
    VK_CHECK(vkResetFences(device, 1, &render_fence));

    VkCommandBufferBeginInfo cmd_begin_info = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    cmd_begin_info.flags                    = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    VK_CHECK(vkBeginCommandBuffer(cmd, &cmd_begin_info));

    VkClearValue clear_value;
    clear_value.color = {{0.1f, 0.1f, 0.1f, 1.0f}};

    VkRenderPassBeginInfo render_pass_begin_info = {VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
    render_pass_begin_info.clearValueCount       = 1;
    render_pass_begin_info.pClearValues          = &clear_value;
    render_pass_begin_info.framebuffer           = framebuffers.at(swapchainIndex);
    render_pass_begin_info.renderPass            = render_pass;
    render_pass_begin_info.renderArea.offset     = {0, 0};
    render_pass_begin_info.renderArea.extent     = extent;

    vkCmdBeginRenderPass(cmd, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, triangle_pipeline);

    VkViewport viewport = {};
    viewport.x          = 0;
    viewport.y          = 0;
    viewport.width      = static_cast<f32>(extent.width);
    viewport.height     = static_cast<f32>(extent.height);

    vkCmdSetViewport(cmd, 0, 1, &viewport);

    VkRect2D scissor = {};
    scissor.extent   = extent;
    scissor.offset   = {0, 0};

    vkCmdSetScissor(cmd, 0, 1, &scissor);

    vkCmdDraw(cmd, 3, 1, 0, 0);

    vkCmdEndRenderPass(cmd);

    VK_CHECK(vkEndCommandBuffer(cmd));

    VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo submit         = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submit.pWaitDstStageMask    = &waitStage;
    submit.commandBufferCount   = 1;
    submit.pCommandBuffers      = &cmd;
    submit.waitSemaphoreCount   = 1;
    submit.pWaitSemaphores      = &present_semaphore;
    submit.signalSemaphoreCount = 1;
    submit.pSignalSemaphores    = &render_semaphore;

    VK_CHECK(vkQueueSubmit(graphics_queue, 1, &submit, render_fence));

    VkPresentInfoKHR present_info   = {VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
    present_info.swapchainCount     = 1;
    present_info.pSwapchains        = &swapchain.swapchain;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores    = &render_semaphore;
    present_info.pImageIndices      = &swapchainIndex;

    ok = vkQueuePresentKHR(present_queue, &present_info);
    if (ok == VK_ERROR_OUT_OF_DATE_KHR || ok == VK_SUBOPTIMAL_KHR)
    {
        recreate_swapchain();
    }
    else if (ok != VK_SUCCESS)
    {
        PFATAL("Failed to present swapchain image!");
    }
}

resources::BufferHandle VulkanDevice::create_buffer(
  const resources::BufferDescriptor& /*descriptor*/)
{
    return {};
}

resources::TextureHandle VulkanDevice::create_texture(
  const resources::TextureDescriptor& /*descriptor*/)
{
    return {};
}

void VulkanDevice::destroy_buffer(resources::BufferHandle /*handle*/)
{
}

void VulkanDevice::destroy_texture(resources::TextureHandle /*handle*/)
{
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
    framebuffers                    = std::vector<VkFramebuffer>(swapchain_images_size);

    for (u32 i = 0; i < swapchain_images_size; ++i)
    {
        framebuffer_info.pAttachments = &swapchain_image_views.at(i);

        VK_CHECK(vkCreateFramebuffer(device, &framebuffer_info, nullptr, &framebuffers.at(i)));
    }
}

} // namespace vulkan
} // namespace pinut
