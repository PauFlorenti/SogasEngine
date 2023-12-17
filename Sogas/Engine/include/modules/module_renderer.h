#pragma once

#include <modules/module.h>
#include <render_device.h> // TODO this should be something like pinut/device or render/device ??

namespace sogas
{
namespace modules
{
class RendererModule : public IModule
{
  public:
    RendererModule() = delete;
    RendererModule(const std::string& name, void* window) : IModule(name), window_handle(window)
    {
    }
    ~RendererModule() = default;

    pinut::GPUDevice* get_device()
    {
        return renderer;
    }

  protected:
    bool start() override;
    void stop() override;
    void update(f32 /*delta_time*/) override{};
    void render() override;
    void render_ui() override{};
    void render_debug() override{};
    void resize_window(u32 width, u32 height) override;

  private:
    //! Not a shared_ptr<pinut::GPUDevice> due to C26495.
    //! Variable not initialized and cannot initialize an abstract class.
    pinut::GPUDevice* renderer = nullptr;

    void* window_handle = nullptr;
};
} // namespace modules
} // namespace sogas
