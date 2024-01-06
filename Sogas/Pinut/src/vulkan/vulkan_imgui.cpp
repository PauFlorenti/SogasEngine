#include "pch.hpp"

#include <imgui/backends/imgui_impl_vulkan.h>
#include <imgui/backends/imgui_impl_win32.h>

#include <vulkan/vulkan_device.h>
#include <vulkan/vulkan_imgui.h>

namespace pinut::vulkan
{
static ImguiState* state = nullptr;

void init_imgui(const VulkanContext& context)
{
    state = new ImguiState();

    state->device = context.device;
    state->gpu    = context.gpu;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    VkDescriptorPoolSize descriptor_pool_size = {};
    descriptor_pool_size.type                 = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptor_pool_size.descriptorCount      = 1;

    VkDescriptorPoolCreateInfo pool_info = {VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
    pool_info.poolSizeCount              = 1;
    pool_info.pPoolSizes                 = &descriptor_pool_size;
    pool_info.maxSets                    = 1;
    pool_info.flags                      = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

    VK_CHECK(vkCreateDescriptorPool(state->device, &pool_info, nullptr, &state->descriptor_pool));

    ImGui_ImplVulkan_InitInfo imgui_vulkan_info = {};
    imgui_vulkan_info.Instance                  = context.instance;
    imgui_vulkan_info.PhysicalDevice            = context.gpu;
    imgui_vulkan_info.Device                    = context.device;
    imgui_vulkan_info.QueueFamily               = context.graphics_queue_index;
    imgui_vulkan_info.Queue                     = context.graphics_queue;
    imgui_vulkan_info.DescriptorPool            = state->descriptor_pool;
    imgui_vulkan_info.MinImageCount             = 3;
    imgui_vulkan_info.ImageCount                = 3;

    ImGuiIO& io                = ImGui::GetIO();
    io.DisplaySize             = ImVec2(1280.0f, 720.0f);
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

    ImGui_ImplWin32_Init(context.window_handle);
    if (!ImGui_ImplVulkan_Init(&imgui_vulkan_info, context.render_pass))
    {
        PFATAL("Failed to initialize imgui for vulkan backend.");
    }

    ImGui_ImplVulkan_CreateFontsTexture();
}

void render_imgui(const VkCommandBuffer& cmd)
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    bool window_open = true;
    ImGui::ShowDemoWindow(&window_open);
    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
    ImGui::EndFrame();
}

void shutdown_imgui()
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    vkDestroyDescriptorPool(state->device, state->descriptor_pool, nullptr);
    delete state;
}
} // namespace pinut::vulkan
