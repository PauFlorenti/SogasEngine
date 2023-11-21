#include <vulkan/vulkan_pipeline_builder.h>

namespace pinut
{
namespace vulkan
{
VkPipeline VulkanPipelineBuilder::build_pipeline(VkDevice device, VkRenderPass render_pass)
{
    VkPipelineViewportStateCreateInfo viewport_info = {
      VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
    viewport_info.viewportCount = 1;
    viewport_info.pViewports    = &viewport;
    viewport_info.scissorCount  = 1;
    viewport_info.pScissors     = &scissor;

    VkPipelineColorBlendStateCreateInfo color_blend_info = {
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
    color_blend_info.logicOpEnable   = VK_FALSE;
    color_blend_info.logicOp         = VK_LOGIC_OP_COPY;
    color_blend_info.attachmentCount = 1;
    color_blend_info.pAttachments    = &color_blend_attachment;

    VkGraphicsPipelineCreateInfo info = {VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
    info.stageCount                   = static_cast<u32>(shader_stages.size());
    info.pStages                      = shader_stages.data();
    info.pVertexInputState            = &vertex_input;
    info.pInputAssemblyState          = &assembly_input;
    info.pViewportState               = &viewport_info;
    info.pRasterizationState          = &raster;
    info.pMultisampleState            = &multisampling;
    info.pColorBlendState             = &color_blend_info;
    info.layout                       = pipeline_layout;
    info.renderPass                   = render_pass;
    info.subpass                      = 0;
    info.basePipelineHandle           = VK_NULL_HANDLE;

    VkPipeline pipeline;
    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &info, nullptr, &pipeline) !=
        VK_SUCCESS)
    {
        PERROR("Failed to create pipeline");
        return VK_NULL_HANDLE;
    }

    return pipeline;
}
} // namespace vulkan
} // namespace pinut
