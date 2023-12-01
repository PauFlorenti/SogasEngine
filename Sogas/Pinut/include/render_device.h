#pragma once

#include <render_types.h>
#include <resources/buffer.h>
#include <resources/commandbuffer.h>
#include <resources/pipeline.h>
#include <resources/renderpass.h>
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
    virtual void resize(u32 width, u32 height)            = 0;

    virtual resources::BufferHandle create_buffer(
      const resources::BufferDescriptor& descriptor) = 0;
    virtual resources::TextureHandle create_texture(
      const resources::TextureDescriptor& descriptor) = 0;
    virtual resources::RenderPassHandle create_renderpass(
      const resources::RenderPassDescriptor& descriptor) = 0;

    // TODO return handle for future reference outside backend.
    virtual void create_pipeline(const resources::PipelineDescriptor& descriptor) = 0;

    virtual void begin_frame() = 0;
    virtual void end_frame()   = 0;

    virtual resources::CommandBuffer* get_command_buffer(bool begin) = 0;

    virtual void* map_buffer(const u32 buffer_index, const u32 size) = 0;
    virtual void  unmap_buffer(const u32 buffer_index)               = 0;

    virtual void destroy_buffer(resources::BufferHandle handle)   = 0;
    virtual void destroy_texture(resources::TextureHandle handle) = 0;
};
} // namespace pinut
