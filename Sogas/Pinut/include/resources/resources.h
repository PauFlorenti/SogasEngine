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

struct DescriptorSetHandle
{
    ResourceHandle id;
};

struct DescriptorSetLayoutHandle
{
    ResourceHandle id;
};

static const BufferHandle              invalid_buffer{INVALID_ID};
static const TextureHandle             invalid_texture{INVALID_ID};
static const ShaderStateHandle         invalid_shader_state{INVALID_ID};
static const RenderPassHandle          invalid_render_pass{INVALID_ID};
static const DescriptorSetHandle       invalid_descriptor_set{INVALID_ID};
static const DescriptorSetLayoutHandle invalid_descriptor_set_layout{INVALID_ID};

enum class ResourceDestroyType
{
    BUFFER = 0,
    TEXTURE,
    SHADER_STATE,
    RENDER_PASS,
    DESCRIPTOR_SET,
    DESCRIPTOR_SET_LAYOUT
};

struct ResourceDeletion
{
    ResourceDestroyType type;
    ResourceHandle      handle;
};

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

enum class DescriptorType
{
    SAMPLER,
    COMBINED_IMAGE_SAMPLER,
    SAMPLED_IMAGE,
    STORAGE_IMAGE,
    UNIFORM,
    STORAGE,
    UNIFORM_DYNAMIC,
    STORAGE_DYNAMIC,
    INPUT_ATTACHMENT,
    COUNT
};

enum class ShaderStageType
{
    VERTEX = 0,
    GEOMETRY,
    FRAGMENT,
    COMPUTE,
    MAX_SHADER_TYPE
};
} // namespace resources
} // namespace pinut
