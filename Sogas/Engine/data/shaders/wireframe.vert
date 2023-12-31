#version 450

layout (location = 0) in vec3 position;

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
}
