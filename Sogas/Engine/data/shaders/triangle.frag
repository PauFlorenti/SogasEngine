#version 450

layout (location = 0) out vec4 outFragmentColor;

layout (location = 0) in vec3 inColor;
layout (location = 1) in vec2 inUv;

layout (binding = 1) uniform sampler2D color_texture;

void main()
{
    outFragmentColor = texture(color_texture, inUv);
}
