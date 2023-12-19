#version 450

layout (location = 0) out vec4 outFragmentColor;

layout (location = 0) in vec3 inColor;
layout (location = 1) in vec2 inUv;

layout (binding = 0, set = 1) uniform material_instance {
    vec3 color;
} u_material_instance;
layout (binding = 1, set = 1) uniform sampler2D albedo_texture;
layout (binding = 2, set = 1) uniform sampler2D normal_texture;

void main()
{
    vec4 albedo = vec4(u_material_instance.color, 1.0f) * texture(albedo_texture, inUv);
    vec3 normal = texture(normal_texture, inUv).xyz;

    outFragmentColor = texture(normal_texture, inUv);
}
