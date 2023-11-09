#include <render_device.h>
#include <vulkan/vulkan_device.h>

namespace pinut
{
DeviceDescriptor& DeviceDescriptor::set_window(u32 width_, u32 height_, void* window_)
{
    width  = static_cast<u16>(width_);
    height = static_cast<u16>(height_);
    window = window_;

    return *this;
}

static GPUDevice* create_vulkan_device()
{
    return new vulkan::VulkanDevice();
}

GPUDevice* GPUDevice::create(GraphicsAPI api)
{
    switch (api)
    {
        case GraphicsAPI::Vulkan:
            return create_vulkan_device();
            break;

        default:
            PFATAL("No valid graphics API.");
            return nullptr;
            break;
    }
}
} // namespace pinut
