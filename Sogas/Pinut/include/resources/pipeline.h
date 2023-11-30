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

struct PipelineDescriptor
{
    const char*             name = nullptr;
    ShaderStateDescriptor   shader_state;
    RasterizationDescriptor rasterization;
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
