#pragma once

#include <resources/commandbuffer.h>
#include <resources/pipeline.h>
#include <vulkan/vulkan.h>

namespace pinut
{
namespace vulkan
{
class VulkanCommandBuffer : public resources::CommandBuffer
{
  public:
    void bind_pass(std::string pass) override;
    void bind_pipeline(std::string pipeline) override;

    void set_viewport(const resources::Viewport* viewport) override;
    void set_scissors(const resources::Rect* scissors) override;

    void clear(f32 red, f32 green, f32 blue, f32 alpha) override;

    void draw(u32 first_vertex, u32 vertex_count, u32 first_instance, u32 instance_count) override;
    void draw_indexed(u32 first_index,
                      u32 index_count,
                      u32 first_instance,
                      u32 instance_count,
                      u32 vertex_offset) override;

    VkCommandBuffer cmd = VK_NULL_HANDLE;

  private:
    VkClearValue clear_value;
};
} // namespace vulkan
} // namespace pinut
