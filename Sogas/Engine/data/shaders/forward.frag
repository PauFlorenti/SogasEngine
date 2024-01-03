#version 450

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inColor;
layout (location = 3) in vec2 inUv;

layout (location = 0) out vec4 outFragmentColor;

const int LIGHT_COUNT = 3;
struct Light
{
    vec3  position;
    float intensity;
    vec3  color;
    float radius;
};

layout (binding = 1, set = 0) uniform light {
    Light l[LIGHT_COUNT];
} u_light;

layout (binding = 0, set = 1) uniform material_instance {
    vec3 color;
} u_material_instance;
layout (binding = 1, set = 1) uniform sampler2D albedo_texture;
layout (binding = 2, set = 1) uniform sampler2D normal_texture;

void main()
{
    vec3 N      = normalize(inNormal);
    vec4 light  = vec4(0.f);
    vec4 albedo = vec4(inColor, 1.0f) * texture(albedo_texture, inUv);
    vec3 normal = texture(normal_texture, inUv).xyz;

    vec4 material_color = albedo * vec4(u_material_instance.color, 1.0);

    for (int light_index = 0; light_index < LIGHT_COUNT; light_index++)
    {
        Light l = u_light.l[light_index];
        
        vec3 L = l.position - inPosition;
        float distance_to_light = length(L);

        if (distance_to_light > l.radius)
            continue;

        L = normalize(L);
        float dotNL = normalize(dot(N, L));

        float attenuation = 0.0f;
        attenuation += l.radius / distance_to_light;

        if (dotNL > 0.0f) { // Pixel visible by light.
            light += attenuation * vec4(l.color, 1) * l.intensity * dotNL;
        }
    }

    outFragmentColor = light * material_color;
}
