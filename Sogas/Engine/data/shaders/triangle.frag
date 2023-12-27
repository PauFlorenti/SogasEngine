#version 450

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inColor;
layout (location = 3) in vec2 inUv;

layout (location = 0) out vec4 outFragmentColor;

layout (binding = 1, set = 0) uniform light {
    vec3 position;
    float intensity;
    vec3 color;
    float max_distance;
} u_light;

layout (binding = 0, set = 1) uniform material_instance {
    vec3 color;
} u_material_instance;
layout (binding = 1, set = 1) uniform sampler2D albedo_texture;
layout (binding = 2, set = 1) uniform sampler2D normal_texture;

void main()
{
    vec3 L = u_light.position - inPosition;
    float distance_to_light = length(L);
    L = normalize(L);
    vec3 N = normalize(inNormal);
    float dotNL = normalize(dot(N, L));

    float attenuation = 0.0f;
    attenuation += u_light.max_distance / distance_to_light;

    vec4 color = vec4(0.1f);
    vec4 albedo = vec4(u_material_instance.color * inColor, 1.0f) * texture(albedo_texture, inUv);
    vec3 normal = texture(normal_texture, inUv).xyz;

    if (dotNL > 0.0f) { // Pixel visible by light.
        color += attenuation * u_light.intensity * dotNL;
    }

    outFragmentColor = color * albedo;
}
