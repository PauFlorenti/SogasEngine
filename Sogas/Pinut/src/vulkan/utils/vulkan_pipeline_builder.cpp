#include "pch.hpp"

#include <resources/pipeline.h>
#include <resources/shader_state.h>
#include <vulkan/utils/vulkan_pipeline_builder.h>
#include <vulkan/utils/vulkan_shader_loader.h>
#include <vulkan/vulkan_device.h>

namespace
{
static bool create_shader_module(VkDevice                             device,
                                 const pinut::resources::ShaderStage& shader_stage,
                                 VkShaderModule&                      shader_module)
{
    VkShaderModuleCreateInfo info = {VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
    info.codeSize                 = shader_stage.code.size() * sizeof(u32);
    info.pCode                    = shader_stage.code.data();

    if (vkCreateShaderModule(device, &info, nullptr, &shader_module) != VK_SUCCESS)
    {
        PERROR("Failed to create shader module!");
        return false;
    }

    return true;
}
} // namespace

namespace pinut
{
namespace vulkan
{
bool VulkanPipeline::build_pipeline(VkDevice                       device,
                                    resources::PipelineDescriptor* descriptor,
                                    VkRenderPass                   render_pass)
{
    VulkanShaderState pipeline_shader_stages_info;
    if (VulkanDevice::shaders.contains(descriptor->shader_state.name))
    {
        pipeline_shader_stages_info = VulkanDevice::shaders.at(descriptor->shader_state.name);
    }
    else
    {
        const u32 shader_stages_count =
          static_cast<u32>(descriptor->shader_state.number_used_stages);
        pipeline_shader_stages_info.resize(shader_stages_count);

        u32 index = 0;
        for (u32 i = 0; i < static_cast<u32>(resources::ShaderStageType::MAX_SHADER_TYPE); ++i)
        {
            if (descriptor->shader_state.shader_stages[i].code.empty())
            {
                continue;
            }

            const auto&    shader_stage = descriptor->shader_state.shader_stages[i];
            VkShaderModule module;
            if (!create_shader_module(device, shader_stage, module))
            {
                PERROR("Failed to create shader module!");
                return false;
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

        VulkanDevice::shaders.insert({descriptor->shader_state.name, pipeline_shader_stages_info});
    }

    VkPipelineRasterizationStateCreateInfo rasterization_info = {
      VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
    rasterization_info.cullMode = vulkan::get_cull_mode_flag(descriptor->rasterization.cull_mode);
    rasterization_info.frontFace =
      vulkan::get_front_face_flag(descriptor->rasterization.front_face);
    rasterization_info.polygonMode =
      vulkan::get_cull_mode_flag(descriptor->rasterization.fill_mode);
    rasterization_info.lineWidth               = descriptor->rasterization.line_width;
    rasterization_info.depthClampEnable        = VK_FALSE;
    rasterization_info.rasterizerDiscardEnable = VK_FALSE;
    rasterization_info.depthBiasEnable         = VK_FALSE;
    rasterization_info.depthBiasClamp          = 0.0f;
    rasterization_info.depthBiasConstantFactor = 0.0f;
    rasterization_info.depthBiasSlopeFactor    = 0.0f;

    VkPipelineInputAssemblyStateCreateInfo assembly_info = {
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
    assembly_info.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    assembly_info.primitiveRestartEnable = VK_FALSE;

    VkPipelineVertexInputStateCreateInfo vertex_input_info = {
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};

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
    viewport.x        = descriptor->viewport.viewport.x;
    viewport.y        = descriptor->viewport.viewport.y;
    viewport.width    = static_cast<float>(descriptor->viewport.viewport.width);
    viewport.height   = static_cast<float>(descriptor->viewport.viewport.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor;
    scissor.offset = {0, 0};
    scissor.extent = {descriptor->viewport.rect.w, descriptor->viewport.rect.h};

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

    VkPipelineLayoutCreateInfo pipeline_layout_info = {
      VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
    VK_CHECK(
      vkCreatePipelineLayout(device, &pipeline_layout_info, nullptr, &pipeline.pipeline_layout));

    VkGraphicsPipelineCreateInfo info = {VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
    info.stageCount                   = static_cast<u32>(pipeline_shader_stages_info.size());
    info.pStages                      = pipeline_shader_stages_info.data();
    info.pVertexInputState            = &vertex_input_info;
    info.pInputAssemblyState          = &assembly_info;
    info.pViewportState               = &viewport_info;
    info.pRasterizationState          = &rasterization_info;
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
        PERROR("Failed to create pipeline");
        return false;
    }

    VulkanDevice::pipelines.insert({descriptor->name, pipeline});

    return true;
}
} // namespace vulkan
} // namespace pinut
