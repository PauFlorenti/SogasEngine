#include <modules/module_renderer.h>

#include <vulkan/vulkan_device.h>

namespace sogas
{
namespace modules
{
bool RendererModule::start()
{
    pinut::DeviceDescriptor descriptor;
    descriptor.set_window(1280, 720, window_handle);

    renderer = pinut::GPUDevice::create(pinut::GraphicsAPI::Vulkan);
    renderer->init(descriptor);

    return true;
}

void RendererModule::stop()
{
    renderer->shutdown();
}

void RendererModule::update(f32 /*delta_time*/)
{
    // TODO: renderer should not have an update function
}

void RendererModule::render()
{
    renderer->update();
}
void RendererModule::resize_window(u32 width, u32 height)
{
    renderer->resize(width, height);
}
} // namespace modules
} // namespace sogas
