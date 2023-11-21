#include <vulkan/utils/vulkan_shader_loader.h>

#include <fstream>

namespace pinut
{
namespace vulkan
{
bool VulkanShaderLoader::load_shader_module(const char* filepath)
{
    std::ifstream file(filepath, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        return false;
    }

    file_size = static_cast<size_t>(file.tellg());
    buffer.resize(file_size / sizeof(u32));

    file.seekg(0);
    file.read((char*)(buffer.data()), file_size);
    file.close();

    return true;
}

VkShaderModule VulkanShaderLoader::create_shader_module(VkDevice device, const char* filepath)
{
    if (!load_shader_module(filepath))
    {
        PFATAL("Failed to read shader '%s'", filepath);
    }

    ASSERT(!buffer.empty());

    VkShaderModuleCreateInfo info = {VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
    info.codeSize                 = buffer.size() * sizeof(u32);
    info.pCode                    = buffer.data();

    if (vkCreateShaderModule(device, &info, nullptr, &module) != VK_SUCCESS)
    {
        PERROR("Failed to create shader module!");
        return nullptr;
    }

    return module;
}
} // namespace vulkan
} // namespace pinut
