#pragma once

#include <vulkan/vulkan.h>

namespace pinut
{
namespace vulkan
{
class VulkanPipelineBuilder
{
  public:
    std::vector<VkPipelineShaderStageCreateInfo> shader_stages;
    VkPipelineVertexInputStateCreateInfo         vertex_input;
    VkPipelineInputAssemblyStateCreateInfo       assembly_input;
    VkViewport                                   viewport;
    VkRect2D                                     scissor;
    VkPipelineRasterizationStateCreateInfo       raster;
    VkPipelineColorBlendAttachmentState          color_blend_attachment;
    VkPipelineMultisampleStateCreateInfo         multisampling;
    VkPipelineLayout                             pipeline_layout;

    VkPipeline build_pipeline(VkDevice device, VkRenderPass render_pass);
};
} // namespace vulkan
} // namespace pinut
