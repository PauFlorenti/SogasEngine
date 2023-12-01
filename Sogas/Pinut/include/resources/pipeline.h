#pragma once

#include <resources/shader_state.h>

namespace pinut
{
namespace resources
{

enum CullMode
{
    NONE = 0,
    FRONT,
    BACK,
    FRONT_AND_BACK
};

enum FrontFace
{
    COUNTER_CLOCKWISE = 0,
    CLOCKWISE
};

enum FillMode
{
    FILL = 0,
    LINE,
    POINT
};

struct RasterizationDescriptor
{
    CullMode  cull_mode  = CullMode::NONE;
    FrontFace front_face = FrontFace::COUNTER_CLOCKWISE;
    FillMode  fill_mode  = FillMode::FILL;
    float     line_width = 1.0f;
};

struct Viewport
{
    f32 x;
    f32 y;
    f32 width;
    f32 height;
    f32 min_depth = 0.0f;
    f32 max_depth = 1.0f;
};

struct Rect
{
    f32 offset_x;
    f32 offset_y;
    u32 w;
    u32 h;
};

struct ViewportDescriptor
{
    Viewport viewport;
    Rect     rect;
};

struct VertexAttribute
{
    u16 location = 0;
    u16 binding  = 0;
    u32 offset   = 0;
    // TODO Add the component format. Float3/Vec3 at the moment.
};

struct VertexStream
{
    u16 binding = 0;
    u16 stride  = 0;
    // TODO Add the vertex input rate [per vertex or per instance].
};

struct VertexInputDescriptor
{
    // TODO At the moment just two, should not be hardcoded.
    u32             stream_count    = 0;
    u32             attribute_count = 0;
    VertexStream    streams[2];
    VertexAttribute attributes[2];

    VertexInputDescriptor& reset()
    {
        stream_count    = 0;
        attribute_count = 0;

        return *this;
    }

    VertexInputDescriptor& add_vertex_stream(const VertexStream& stream)
    {
        ASSERT(stream_count < 2);

        streams[stream_count++] = stream;

        return *this;
    }

    VertexInputDescriptor& add_vertex_attribute(const VertexAttribute& attribute)
    {
        ASSERT(attribute_count < 2);

        attributes[attribute_count++] = attribute;

        return *this;
    }
};

struct PipelineDescriptor
{
    const char*             name = nullptr;
    ShaderStateDescriptor   shader_state;
    RasterizationDescriptor rasterization;
    VertexInputDescriptor   vertex_input;
    ViewportDescriptor      viewport;

    PipelineDescriptor& add_name(const char* new_name)
    {
        if (name != nullptr)
        {
            PWARN("Overriding pipeline descriptor name.");
        }

        name = new_name;
        return *this;
    }

    PipelineDescriptor& add_shader_state(ShaderStateDescriptor& new_shader_state)
    {
        shader_state = new_shader_state;
        return *this;
    }

    PipelineDescriptor& add_rasterization(RasterizationDescriptor& new_rasterization)
    {
        rasterization = new_rasterization;
        return *this;
    }

    PipelineDescriptor& add_viewport(ViewportDescriptor& new_viewport)
    {
        viewport = new_viewport;
        return *this;
    }
};
} // namespace resources
} // namespace pinut
