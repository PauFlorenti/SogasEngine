#pragma once

#include <resources/resources.h>

namespace pinut
{
namespace resources
{
struct ShaderStage
{
    std::vector<u32> code;
    ShaderStageType  type;
};

struct ShaderStateDescriptor
{
    const char* name = nullptr;
    ShaderStage shader_stages[static_cast<u32>(ShaderStageType::MAX_SHADER_TYPE)];
    u32         number_used_stages = 0;

    ShaderStateDescriptor& add_name(const char* new_name)
    {
        if (name != nullptr)
        {
            PWARN("Overriding shader state name.");
        }

        name = new_name;
        return *this;
    }

    ShaderStateDescriptor& add_shader_stage(ShaderStage shader_stage)
    {
        if (!shader_stages[static_cast<u32>(shader_stage.type)].code.empty())
        {
            PWARN("Overriding shader stage!");
        }
        else
        {
            number_used_stages++;
        }

        shader_stages[static_cast<u32>(shader_stage.type)] = shader_stage;
        return *this;
    }
};

struct DescriptorSetBindingDescriptor
{
    u32             binding         = 0;
    u32             count           = 0;
    ShaderStageType shader_stage    = ShaderStageType::MAX_SHADER_TYPE;
    DescriptorType  descriptor_type = DescriptorType::COUNT;
};

struct DescriptorSetLayoutDescriptor
{
    u32                            binding_count = 0;
    DescriptorSetBindingDescriptor bindings[16];
    const char*                    name;

    DescriptorSetLayoutDescriptor& add_name(const char* new_name)
    {
        name = new_name;
        return *this;
    }

    DescriptorSetLayoutDescriptor& add_binding(const DescriptorSetBindingDescriptor& binding)
    {
        bindings[binding_count++] = binding;
        return *this;
    }
};

static const u8 MAX_DESCRIPTOR_PER_SET = 16;

struct DescriptorSetDescriptor
{
    // TODO Add samplers resources.
    //SamplerHandle  samplers[MAX_DESCRIPTOR_PER_SET];
    ResourceHandle resources[MAX_DESCRIPTOR_PER_SET];
    u16            bindings[MAX_DESCRIPTOR_PER_SET];
    u16            resources_used{0};

    DescriptorSetLayoutHandle layout_handle = {INVALID_ID};

    std::string name;

    DescriptorSetDescriptor& reset()
    {
        resources_used = 0;
        name           = "";
        return *this;
    }

    DescriptorSetDescriptor& set_layout(const DescriptorSetLayoutHandle new_layout_handle)
    {
        ASSERT(new_layout_handle.id != INVALID_ID);
        layout_handle = new_layout_handle;
        return *this;
    }

    DescriptorSetDescriptor& add_buffer(const BufferHandle& buffer, const u16 binding)
    {
        ASSERT(buffer.id != INVALID_ID);

        // TODO Samplers
        bindings[resources_used]    = binding;
        resources[resources_used++] = buffer.id;
        return *this;
    }

    DescriptorSetDescriptor& add_texture(const TextureHandle& texture, const u16 binding)
    {
        // TODO Add sampler to be used.
        ASSERT(texture.id != INVALID_ID);

        bindings[resources_used]    = binding;
        resources[resources_used++] = texture.id;

        return *this;
    }

    DescriptorSetDescriptor& set_name(const char* new_name)
    {
        name = new_name;
        return *this;
    }
};

struct PushConstantDescriptor
{
    ShaderStageType stage  = ShaderStageType::MAX_SHADER_TYPE;
    u32             size   = 0;
    u32             offset = 0;
};

} // namespace resources
} // namespace pinut
