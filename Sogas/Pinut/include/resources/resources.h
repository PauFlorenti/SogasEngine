#pragma once

namespace pinut
{
namespace resources
{

using ResourceHandle = u32;

struct BufferHandle
{
    ResourceHandle id;
};

struct TextureHandle
{
    ResourceHandle id;
};

struct ShaderStateHandle
{
    ResourceHandle id;
};

struct RenderPassHandle
{
    ResourceHandle id;
};

static const BufferHandle      invalid_buffer{INVALID_ID};
static const TextureHandle     invalid_texture{INVALID_ID};
static const ShaderStateHandle invalid_shader_state{INVALID_ID};
static const RenderPassHandle  invalid_render_pass{INVALID_ID};

} // namespace resources
} // namespace pinut
