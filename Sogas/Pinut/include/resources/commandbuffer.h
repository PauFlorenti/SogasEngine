#pragma once

#include <resources/resources.h>

namespace pinut
{
class GPUDevice;
namespace resources
{
struct Rect;
struct Viewport;
class CommandBuffer
{
  public:
    virtual void bind_pass(std::string pass)         = 0;
    virtual void bind_pipeline(std::string pipeline) = 0;

    virtual void set_viewport(const Viewport* viewport) = 0;
    virtual void set_scissors(const Rect* scissors)     = 0;

    virtual void clear(f32 red, f32 green, f32 blue, f32 alpha) = 0;

    virtual void draw(u32 first_vertex,
                      u32 vertex_count,
                      u32 first_instance,
                      u32 instance_count)        = 0;
    virtual void draw_indexed(u32 first_index,
                              u32 index_count,
                              u32 first_instance,
                              u32 instance_count,
                              u32 vertex_offset) = 0;

    virtual void bind_descriptor_set(const u32 descriptor_set_id)         = 0;
    virtual void bind_vertex_buffer(const BufferHandle handle,
                                    const u32          binding,
                                    const u32          offset)                     = 0;
    virtual void bind_index_buffer(const BufferHandle         handle,
                                   resources::BufferIndexType index_type) = 0;

    GPUDevice* device = nullptr;
};
} // namespace resources
} // namespace pinut
