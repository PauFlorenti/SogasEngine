#pragma once

#include <vulkan/vulkan.h>

namespace pinut
{
namespace vulkan
{
using VulkanShaderState = std::vector<VkPipelineShaderStageCreateInfo>;

class VulkanShaderLoader
{
  public:
    VulkanShaderLoader() = default;

    static bool      load_shader_module(const char* filepath, std::vector<u32>& out_buffer);
    VkShaderModule   create_shader_module(VkDevice device, const char* filepath);
    std::vector<u32> get_buffer() const
    {
        return buffer;
    }

  private:
    bool load_shader_module(const char* filepath);

    size_t           file_size = 0;
    std::vector<u32> buffer;
    VkShaderModule   module = VK_NULL_HANDLE;
};
} // namespace vulkan
} // namespace pinut
