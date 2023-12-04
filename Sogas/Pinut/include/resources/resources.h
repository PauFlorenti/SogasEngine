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

enum class VertexInputFormatType
{
    FLOAT,
    INT,
    UINT,
    BOOL,
    VEC2,
    VEC3,
    VEC4,
    COUNT
};

enum class VertexInputRate
{
    PER_VERTEX,
    PER_INSTANCE,
    COUNT
};

enum class TopologyType
{
    POINT,
    LINE,
    LINE_STRIP,
    TRIANGLE,
    TRIANGLE_STRIP,
    TRIANGLE_FAN,
    COUNT
};

enum class BufferIndexType
{
    UINT16,
    UINT32,
    COUNT
};
} // namespace resources
} // namespace pinut
