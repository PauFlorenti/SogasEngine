#include <vulkan/utils/vulkan_render_pass.h>

namespace pinut
{
namespace vulkan
{
void VulkanRenderPass::create_swapchain_renderpass(VkDevice /*device*/,
                                                   VkFormat /*format*/,
                                                   VkRect2D /*extent*/,
                                                   std::vector<VkImageView>& /*views*/)
{
    // VkAttachmentDescription color_attachment = {};
    // color_attachment.format                  = swapchain.surface_format.format;
    // color_attachment.samples                 = VK_SAMPLE_COUNT_1_BIT;
    // color_attachment.loadOp                  = VK_ATTACHMENT_LOAD_OP_CLEAR;
    // color_attachment.storeOp                 = VK_ATTACHMENT_STORE_OP_STORE;
    // color_attachment.stencilLoadOp           = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    // color_attachment.stencilStoreOp          = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    // color_attachment.initialLayout           = VK_IMAGE_LAYOUT_UNDEFINED;
    // color_attachment.finalLayout             = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    // VkAttachmentReference attachment_reference = {};
    // attachment_reference.attachment            = 0;
    // attachment_reference.layout                = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // VkSubpassDescription subpass = {};
    // subpass.colorAttachmentCount = 1;
    // subpass.pColorAttachments    = &attachment_reference;
    // subpass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;

    // VkRenderPassCreateInfo render_pass_info = {VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
    // render_pass_info.attachmentCount        = 1;
    // render_pass_info.pAttachments           = &color_attachment;
    // render_pass_info.subpassCount           = 1;
    // render_pass_info.pSubpasses             = &subpass;

    // VK_CHECK(
    //   vkCreateRenderPass(device, &render_pass_info, nullptr, &renderpass->renderpass_handle));

    // VkFramebufferCreateInfo framebuffer_info = {VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
    // framebuffer_info.renderPass              = renderpass->renderpass_handle;
    // framebuffer_info.width                   = extent.extent.width;
    // framebuffer_info.height                  = extent.extent.height;
    // framebuffer_info.attachmentCount         = 1;
    // framebuffer_info.layers                  = 1;

    // const u32 swapchain_images_size = static_cast<u32>(views.size());
    // renderpass->framebuffers        = std::vector<VkFramebuffer>(swapchain_images_size);

    // for (u32 i = 0; i < swapchain_images_size; ++i)
    // {
    //     framebuffer_info.pAttachments = &views.at(i);

    //     VK_CHECK(
    //       vkCreateFramebuffer(device, &framebuffer_info, nullptr, &renderpass->framebuffers.at(i)));
    // }
}
} // namespace vulkan
} // namespace pinut
