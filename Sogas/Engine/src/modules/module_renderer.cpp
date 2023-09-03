#include <modules/module_renderer.h>

#include <vulkan/vulkan_device.h>

namespace sogas
{
namespace modules
{
bool RendererModule::start()
{
    pinut::DeviceDescriptor descriptor;
    descriptor.set_window(1280, 720, nullptr);

    renderer = pinut::GPUDevice::create(pinut::GraphicsAPI::Vulkan);
    renderer->init({});

    return true;
}

void RendererModule::stop()
{
    renderer->shutdown();
}

void RendererModule::update(f32 /*delta_time*/)
{
}
} // namespace modules
} // namespace sogas