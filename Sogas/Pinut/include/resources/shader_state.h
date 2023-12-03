#pragma once

namespace pinut
{
namespace resources
{
enum class ShaderStageType
{
    VERTEX = 0,
    GEOMETRY,
    FRAGMENT,
    COMPUTE,
    MAX_SHADER_TYPE
};

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

    DescriptorSetLayoutDescriptor& add_binding(const DescriptorSetBindingDescriptor* new_binding)
    {
        bindings[binding_count++] = *new_binding;
        return *this;
    }
};

struct DescriptorSetDescriptor
{
    // TODO At the moment only one buffer. Multiple resources per set in the future.
    u32 buffer_index = INVALID_ID;
    u32 binding      = INVALID_ID;

    u32 layout_id = INVALID_ID;

    std::string name;

    DescriptorSetDescriptor& reset()
    {
        buffer_index = INVALID_ID;
        binding      = INVALID_ID;
        name         = "";
        return *this;
    }

    DescriptorSetDescriptor& set_layout(const u32 new_layout_id)
    {
        ASSERT(new_layout_id != INVALID_ID);
        layout_id = new_layout_id;
        return *this;
    }

    DescriptorSetDescriptor& add_buffer(const BufferHandle& buffer, const u16 new_binding)
    {
        ASSERT(buffer.id != INVALID_ID);

        buffer_index = buffer.id;
        binding      = static_cast<u32>(new_binding);
        return *this;
    }

    DescriptorSetDescriptor& set_name(const char* new_name)
    {
        name = new_name;
        return *this;
    }
};
} // namespace resources
} // namespace pinut
