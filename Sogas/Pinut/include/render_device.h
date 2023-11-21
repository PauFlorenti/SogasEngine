#pragma once

#include <render_types.h>
#include <resources/buffer.h>
#include <resources/resources.h>
#include <resources/texture.h>

namespace pinut
{
struct DeviceDescriptor
{
    void* window = nullptr;
    u16   width  = 0;
    u16   height = 0;

    DeviceDescriptor& set_window(u32 width, u32 height, void* handle);
};

class GPUDevice
{
  public:
    static GPUDevice* create(GraphicsAPI api);

    //GPUDevice() = default;
    virtual ~GPUDevice(){};

    virtual void init(const DeviceDescriptor& descriptor) = 0;
    virtual void shutdown()                               = 0;

    //! Begin Temporal block
    virtual void update() = 0;
    // End Temporal block

    virtual resources::BufferHandle create_buffer(
      const resources::BufferDescriptor& descriptor) = 0;
    virtual resources::TextureHandle create_texture(
      const resources::TextureDescriptor& descriptor) = 0;

    virtual void destroy_buffer(resources::BufferHandle handle)   = 0;
    virtual void destroy_texture(resources::TextureHandle handle) = 0;
};
} // namespace pinut
