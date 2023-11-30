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
} // namespace resources
} // namespace pinut
