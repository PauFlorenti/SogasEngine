#include "pch.hpp"

#include <resources/pipeline.h>
#include <vulkan/utils/vulkan_commandbuffer.h>
#include <vulkan/utils/vulkan_utils.h>
#include <vulkan/vulkan_device.h>

namespace pinut
{
namespace vulkan
{
void VulkanCommandBuffer::bind_pass(std::string pass)
{
    ASSERT(device != nullptr);

    auto vulkan_device = static_cast<VulkanDevice*>(device);
    ASSERT(device != nullptr);

    if (!VulkanDevice::render_passes.contains(pass))
    {
        throw std::runtime_error("Failed to find desired render pass.");
    }

    auto render_pass = VulkanDevice::render_passes.at(pass);

    VkRenderPassBeginInfo render_pass_begin_info = {VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
    render_pass_begin_info.clearValueCount       = 1;
    render_pass_begin_info.pClearValues          = &clear_value;
    render_pass_begin_info.framebuffer =
      render_pass.type == resources::RenderPassType::SWAPCHAIN ?
        vulkan_device->framebuffers[vulkan_device->current_frame] :
        nullptr;
    render_pass_begin_info.renderPass        = render_pass.handle;
    render_pass_begin_info.renderArea.offset = {0, 0};
    render_pass_begin_info.renderArea.extent = {render_pass.width, render_pass.height};

    vkCmdBeginRenderPass(cmd, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanCommandBuffer::bind_pipeline(std::string pipeline_name)
{
    if (!VulkanDevice::pipelines.contains(pipeline_name))
    {
        throw std::runtime_error("Failed to find desired pipeline");
    }

    auto pipeline = VulkanDevice::pipelines.at(pipeline_name);

    current_pipeline_layout = pipeline.pipeline_layout;

    vkCmdBindPipeline(cmd, pipeline.bind_point, pipeline.pipeline);
}

void VulkanCommandBuffer::set_viewport(const resources::Viewport* viewport)
{
    VkViewport vulkan_viewport;

    if (viewport)
    {
        vulkan_viewport.x        = viewport->x;
        vulkan_viewport.y        = viewport->y;
        vulkan_viewport.width    = viewport->width;
        vulkan_viewport.height   = viewport->height;
        vulkan_viewport.minDepth = viewport->min_depth;
        vulkan_viewport.maxDepth = viewport->max_depth;
    }
    else
    {
        auto vulkan_device = dynamic_cast<VulkanDevice*>(device);

        vulkan_viewport.x        = 0.0f;
        vulkan_viewport.y        = 0.0f;
        vulkan_viewport.width    = static_cast<f32>(vulkan_device->get_swapchain_extent().width);
        vulkan_viewport.height   = static_cast<f32>(vulkan_device->get_swapchain_extent().height);
        vulkan_viewport.minDepth = 0.0f;
        vulkan_viewport.maxDepth = 1.0f;
    }

    vkCmdSetViewport(cmd, 0, 1, &vulkan_viewport);
}

void VulkanCommandBuffer::set_scissors(const resources::Rect* new_scissor)
{
    VkRect2D scissor;

    if (new_scissor)
    {
        scissor.extent = {new_scissor->w, new_scissor->h};
        scissor.offset = {static_cast<i32>(new_scissor->offset_x),
                          static_cast<i32>(new_scissor->offset_y)};
    }
    else
    {
        auto vulkan_device = dynamic_cast<VulkanDevice*>(device);
        scissor.extent     = vulkan_device->get_swapchain_extent();
        scissor.offset     = {0, 0};
    }

    vkCmdSetScissor(cmd, 0, 1, &scissor);
}

void VulkanCommandBuffer::clear(f32 red, f32 green, f32 blue, f32 alpha)
{
    clear_value.color = {red, green, blue, alpha};
}

void VulkanCommandBuffer::draw(u32 first_vertex,
                               u32 vertex_count,
                               u32 first_instance,
                               u32 instance_count)
{
    vkCmdDraw(cmd, vertex_count, instance_count, first_vertex, first_instance);
}

void VulkanCommandBuffer::draw_indexed(u32 first_index,
                                       u32 index_count,
                                       u32 first_instance,
                                       u32 instance_count,
                                       u32 vertex_offset)
{
    vkCmdDrawIndexed(cmd, index_count, instance_count, first_index, vertex_offset, first_instance);
}

void VulkanCommandBuffer::bind_descriptor_set(const u32 descriptor_set_id)
{
    const auto iterator = VulkanDevice::descriptor_sets.find(descriptor_set_id);

    if (iterator == VulkanDevice::descriptor_sets.end())
    {
        PFATAL("Failed to find descriptor set to be bound.");
        return;
    }

    vkCmdBindDescriptorSets(cmd,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            current_pipeline_layout,
                            0,
                            1,
                            &iterator->second,
                            0,
                            nullptr);
}

void VulkanCommandBuffer::bind_vertex_buffer(const resources::BufferHandle handle,
                                             const u32                     binding,
                                             const u32                     offset)
{
    auto vulkan_device = dynamic_cast<VulkanDevice*>(device);
    ASSERT(vulkan_device != nullptr);

    auto buffer = vulkan_device->access_buffer(handle);

    if (buffer == nullptr)
    {
        return;
    }

    VkDeviceSize vulkan_offset = {offset};

    vkCmdBindVertexBuffers(cmd, binding, 1, &buffer->buffer, &vulkan_offset);
}

void VulkanCommandBuffer::bind_index_buffer(const resources::BufferHandle handle,
                                            resources::BufferIndexType    index_type)
{
    auto vulkan_device = dynamic_cast<VulkanDevice*>(device);
    ASSERT(vulkan_device != nullptr);

    auto buffer = vulkan_device->access_buffer(handle);

    if (buffer == nullptr)
    {
        return;
    }

    vkCmdBindIndexBuffer(cmd, buffer->buffer, 0, get_buffer_index_type(index_type));
}
} // namespace vulkan
} // namespace pinut
