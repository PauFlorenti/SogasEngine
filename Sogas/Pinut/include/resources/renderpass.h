#pragma once

namespace pinut
{
namespace resources
{

enum class RenderPassType
{
    GEOMETRY = 0,
    COMPUTE,
    SWAPCHAIN,
    MAX_TYPE
};

struct RenderPassDescriptor
{
    std::string name;

    RenderPassType type = RenderPassType::GEOMETRY;
};
} // namespace resources
} // namespace pinut
