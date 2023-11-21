#pragma once

#include <vulkan/vulkan.h>

namespace pinut
{
namespace vulkan
{
class VulkanShaderLoader
{
  public:
    VulkanShaderLoader() = default;

    VkShaderModule create_shader_module(VkDevice device, const char* filepath);

  private:
    bool load_shader_module(const char* filepath);

    size_t           file_size = 0;
    std::vector<u32> buffer;
    VkShaderModule   module = VK_NULL_HANDLE;
};
} // namespace vulkan
} // namespace pinut
