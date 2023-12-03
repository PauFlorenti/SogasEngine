#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

layout (binding = 0) uniform UniformBuffer {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) out vec3 outFragColor;

void main()
{
    mat4 view_projection = ubo.proj * ubo.view;
    gl_Position = view_projection  * ubo.model * vec4(position, 1.0f);
    outFragColor = color;
}
