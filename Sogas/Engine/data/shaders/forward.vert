#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 color;
layout (location = 3) in vec2 uv;

layout (location = 0) out vec3 outPosition;
layout (location = 1) out vec3 outNormal;
layout (location = 2) out vec3 outFragColor;
layout (location = 3) out vec2 outUv;

layout (binding = 0) uniform UniformBuffer {
    mat4 view;
    mat4 proj;
} ubo;

layout (push_constant) uniform push_constant
{
    mat4 model;
} u_push_constant;

void main()
{
    mat4 view_projection = ubo.proj * ubo.view;

    vec4 world_position = u_push_constant.model * vec4(position, 1.0f);
    gl_Position = view_projection * world_position;

    outPosition = world_position.xyz;
    outNormal = (u_push_constant.model * vec4(normal, 1.0f)).xyz;
    outFragColor = color;
    outUv = uv;
}
