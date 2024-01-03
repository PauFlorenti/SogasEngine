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

    // clang-format off
    Handle get_active_camera() const { return active_camera; }
    void set_active_camera(Handle new_camera) { active_camera = new_camera; }
    bool get_wireframe_enabled() const { return is_wireframe; }
    void set_wireframe(bool enable) { is_wireframe = enable; }
    // clang-format on

  protected:
    bool start() override;
    void stop() override;
    void update(f32 /*delta_time*/) override{};
    void render() override;
    void render_ui() override{};
    void render_debug(pinut::resources::CommandBuffer*) override{};
    void resize_window(u32 width, u32 height) override;

  private:
    //! Not a shared_ptr<pinut::GPUDevice> due to C26495.
    //! Variable not initialized and cannot initialize an abstract class.
    pinut::GPUDevice* renderer      = nullptr;
    void*             window_handle = nullptr;
    Handle            active_camera;
    bool              is_wireframe = false;
};
} // namespace modules
} // namespace sogas
