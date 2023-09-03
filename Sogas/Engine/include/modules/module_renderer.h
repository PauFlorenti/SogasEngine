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
    RendererModule(const std::string& name)
    : IModule(name)
    {
    }
    ~RendererModule() = default;

  protected:
    bool start() override;
    void stop() override;
    void update(f32 /*delta_time*/) override;
    void render() override{};
    void render_ui() override{};
    void render_debug() override{};

  private:
      //! Not a shared_ptr<pinut::GPUDevice> due to C26495. 
      //! Variable not initialized and cannot initialize an abstract class.
    pinut::GPUDevice* renderer = nullptr;
};
} // namespace modules
} // namespace sogas