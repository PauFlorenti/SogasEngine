#pragma once

#include <vulkan/vulkan.h>

namespace pinut
{
namespace resources
{
struct PipelineDescriptor;
}
namespace vulkan
{
class VulkanPipeline
{
  public:
    static bool build_pipeline(VkDevice                             device,
                               const resources::PipelineDescriptor* descriptor,
                               VkRenderPass                         render_pass);

    VkPipeline          pipeline        = VK_NULL_HANDLE;
    VkPipelineLayout    pipeline_layout = VK_NULL_HANDLE;
    VkPipelineBindPoint bind_point      = VK_PIPELINE_BIND_POINT_GRAPHICS; // Graphics as default.
};
} // namespace vulkan
} // namespace pinut
