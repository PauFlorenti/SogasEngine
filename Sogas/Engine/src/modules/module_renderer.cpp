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
    renderer->begin_frame();

    auto cmd = renderer->get_command_buffer(true);

    cmd->clear(0.3f, 0.5f, 0.3f, 1.0f);
    cmd->bind_pass("Swapchain_renderpass");
    cmd->bind_pipeline("triangle_pipeline");
    cmd->set_scissors(nullptr);
    cmd->set_viewport(nullptr);

    cmd->draw(0, 3, 0, 1);

    renderer->end_frame();
}
void RendererModule::resize_window(u32 width, u32 height)
{
    renderer->resize(width, height);
}
} // namespace modules
} // namespace sogas
