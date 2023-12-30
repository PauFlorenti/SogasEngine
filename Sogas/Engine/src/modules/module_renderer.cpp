#include "pch.hpp"

#include <components/basic/camera_component.h>
#include <modules/module_renderer.h>
#include <modules/render_manager.h>
#include <resources/pipeline.h>
#include <resources/resources.h>
#include <resources/shader_state.h>

#ifdef _WIN64
#pragma warning(disable : 4615)
#pragma warning(disable : 4389)
#pragma warning(disable : 4244)
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#pragma warning(enable : 4244)
#pragma warning(enable : 4389)
#pragma warning(enable : 4615)
#else
#error "Only win64 platform implemented at the moment."
#endif

#include <resources/mesh.h>

namespace sogas
{
namespace modules
{
static bool read_shader_binary(const std::string& filepath, std::vector<u32>& out_buffer)
{
    out_buffer.clear();

    std::ifstream file(filepath, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        return false;
    }

    auto file_size = static_cast<size_t>(file.tellg());
    ASSERT(file_size > 0);

    out_buffer.resize(file_size / sizeof(u32));

    file.seekg(0);
    file.read((char*)(out_buffer.data()), file_size);
    file.close();

    return true;
}

struct UniformBuffer
{
    glm::mat4 view;
    glm::mat4 proj;
};

struct LightData
{
    glm::vec3 position     = glm::vec3(0.0f, 5.0f, 0.0f);
    f32       intensity    = 1.0f;
    glm::vec3 color        = glm::vec3(1.0f);
    f32       max_distance = 100.0f;
};

using namespace pinut::resources;

struct Material
{
    glm::vec3 color = glm::vec3(1.0f);

    TextureHandle albedo_texture;
    TextureHandle normal_texture;
    TextureHandle metallic_roughness_texture;
    TextureHandle ambient_occlusion_texture;
};

BufferHandle              global_ubo;
BufferHandle              light_ubo;
BufferHandle              material_buffer;
DescriptorSetHandle       descriptor_set_handle;
DescriptorSetHandle       instance_descriptor_set_handle;
DescriptorSetLayoutHandle descriptor_set_layout_handle;
DescriptorSetLayoutHandle instance_descriptor_set_layout_handle;

Material material;

bool RendererModule::start()
{
    PINFO("Starting renderer module.");

    pinut::DeviceDescriptor descriptor;
    descriptor.set_window(1280, 720, window_handle);

    renderer = pinut::GPUDevice::create(pinut::GraphicsAPI::Vulkan);
    renderer->init(descriptor);

    init_default_meshes();

    std::vector<u32> vs_buffer, fs_buffer;

    if (!read_shader_binary("../../Sogas/Engine/data/shaders/bin/triangle.vert.spv", vs_buffer))
    {
        throw std::runtime_error("Failed to load vertex shader data.");
    }

    if (!read_shader_binary("../../Sogas/Engine/data/shaders/bin/triangle.frag.spv", fs_buffer))
    {
        throw std::runtime_error("Failed to load fragment shader data.");
    }

    ShaderStateDescriptor shader_state = {};
    shader_state.add_name("triangle_shader")
      .add_shader_stage({vs_buffer, ShaderStageType::VERTEX})
      .add_shader_stage({fs_buffer, ShaderStageType::FRAGMENT});

    ViewportDescriptor viewport_state = {{0, 0, static_cast<f32>(1280), static_cast<f32>(720)},
                                         {0, 0, 1280, 720}};

    RasterizationDescriptor raster = {};
    raster.cull_mode               = CullMode::NONE;
    raster.line_width              = 1.0f;

    PipelineDescriptor pipeline_descriptor = {};
    pipeline_descriptor.add_name("triangle_pipeline")
      .set_topology(TopologyType::TRIANGLE)
      .add_shader_state(shader_state)
      .add_viewport(viewport_state)
      .add_rasterization(raster);

    pipeline_descriptor.vertex_input.add_vertex_stream({0, sizeof(sogas::Vertex)});
    pipeline_descriptor.vertex_input.add_vertex_attribute(
      {0, 0, offsetof(sogas::Vertex, position), VertexInputFormatType::VEC3});
    pipeline_descriptor.vertex_input.add_vertex_attribute(
      {1, 0, offsetof(sogas::Vertex, normal), VertexInputFormatType::VEC3});
    pipeline_descriptor.vertex_input.add_vertex_attribute(
      {2, 0, offsetof(sogas::Vertex, color), VertexInputFormatType::VEC3});
    pipeline_descriptor.vertex_input.add_vertex_attribute(
      {3, 0, offsetof(sogas::Vertex, uv), VertexInputFormatType::VEC2});

    // CREATING UNIFORM BUFFERS
    global_ubo = renderer->create_buffer({sizeof(UniformBuffer), BufferType::UNIFORM});

    // CREATING TEXTURE DESCRIPTOR
    i32  w, h, c;
    auto pixels =
      stbi_load("D:/Meshes/viking-room/textures/texture.png", &w, &h, &c, STBI_rgb_alpha);

    ASSERT(pixels);

    //u32               texture_data = 0xFF00FFFF;
    TextureDescriptor texture_descriptor{};
    texture_descriptor.width         = w;
    texture_descriptor.height        = h;
    texture_descriptor.channel_count = 4;
    texture_descriptor.data          = pixels;
    material.albedo_texture          = renderer->create_texture(texture_descriptor);

    stbi_image_free(pixels);

    u32               normal_texture_data = 0xFFFFFF00;
    TextureDescriptor normal_texture_descriptor{};
    normal_texture_descriptor.data = &normal_texture_data;
    material.normal_texture        = renderer->create_texture(normal_texture_descriptor);

    LightData light{};
    light.max_distance = 10.0f;
    light_ubo          = renderer->create_buffer({sizeof(LightData), BufferType::UNIFORM, &light});

    glm::vec3 color = glm::vec3(1.0f);
    material_buffer = renderer->create_buffer({sizeof(glm::vec3), BufferType::UNIFORM, &color});

    // CREATING DESCRIPTOR SET LAYOUTS
    DescriptorSetBindingDescriptor binding = {0,
                                              1,
                                              ShaderStageType::VERTEX,
                                              DescriptorType::UNIFORM};

    DescriptorSetBindingDescriptor light_binding = {1,
                                                    1,
                                                    ShaderStageType::FRAGMENT,
                                                    DescriptorType::UNIFORM};

    DescriptorSetBindingDescriptor instance_binding = {0,
                                                       1,
                                                       ShaderStageType::FRAGMENT,
                                                       DescriptorType::UNIFORM};

    DescriptorSetBindingDescriptor texture_binding = {1,
                                                      1,
                                                      ShaderStageType::FRAGMENT,
                                                      DescriptorType::COMBINED_IMAGE_SAMPLER};

    DescriptorSetBindingDescriptor normal_binding = {2,
                                                     1,
                                                     ShaderStageType::FRAGMENT,
                                                     DescriptorType::COMBINED_IMAGE_SAMPLER};

    DescriptorSetLayoutDescriptor descriptor_set_layout_descriptor = {};
    descriptor_set_layout_descriptor.add_binding(binding)
      .add_binding(light_binding)
      .add_name("GLOBAL");

    descriptor_set_layout_handle =
      renderer->create_descriptor_set_layout(descriptor_set_layout_descriptor);

    DescriptorSetLayoutDescriptor instance_descriptor_set_layout_descriptor = {};
    instance_descriptor_set_layout_descriptor.add_binding(instance_binding)
      .add_binding(texture_binding)
      .add_binding(normal_binding)
      .add_name("INSTANCE");

    instance_descriptor_set_layout_handle =
      renderer->create_descriptor_set_layout(instance_descriptor_set_layout_descriptor);

    pipeline_descriptor.add_descriptor_set_layout(descriptor_set_layout_handle);
    pipeline_descriptor.add_descriptor_set_layout(instance_descriptor_set_layout_handle);

    DescriptorSetDescriptor descriptor_set_descriptor = {};
    descriptor_set_descriptor.set_layout(descriptor_set_layout_handle)
      .add_buffer(global_ubo, 0)
      .add_buffer(light_ubo, 1);
    descriptor_set_handle = renderer->create_descriptor_set(descriptor_set_descriptor);

    DescriptorSetDescriptor instance_descriptor_set_descriptor = {};
    instance_descriptor_set_descriptor.set_layout(instance_descriptor_set_layout_handle)
      .add_buffer(material_buffer, 0)
      .add_texture(material.albedo_texture, 1)
      .add_texture(material.normal_texture, 2);

    instance_descriptor_set_handle =
      renderer->create_descriptor_set(instance_descriptor_set_descriptor);

    pipeline_descriptor.add_push_constant({ShaderStageType::VERTEX, sizeof(glm::mat4)});

    renderer->create_pipeline(pipeline_descriptor);

    return true;
}

void RendererModule::stop()
{
    PINFO("Shutting down renderer.");

    renderer->destroy_descriptor_set_layout(descriptor_set_layout_handle);
    renderer->destroy_descriptor_set_layout(instance_descriptor_set_layout_handle);
    renderer->destroy_descriptor_set(descriptor_set_handle);
    renderer->destroy_descriptor_set(instance_descriptor_set_handle);
    renderer->destroy_texture(material.albedo_texture);
    renderer->destroy_texture(material.normal_texture);
    renderer->destroy_buffer(global_ubo);
    renderer->destroy_buffer(light_ubo);
    renderer->destroy_buffer(material_buffer);

    renderer->shutdown();
}

u32 x = 1.0f;

void RendererModule::render()
{
    renderer->begin_frame();

    static u32 current_image = 0;

    UniformBuffer ubo{};

    Entity* camera_entity = active_camera;

    if (!camera_entity)
    {
        camera_entity = get_entity_by_name("camera");
    }

    ASSERT(camera_entity);
    CameraComponent* camera = camera_entity->get<CameraComponent>();
    ASSERT(camera);

    ubo.view = camera->get_view();
    ubo.proj = camera->get_projection();
    ubo.proj[1][1] *= -1;

    auto data = renderer->map_buffer(global_ubo, sizeof(ubo));
    memcpy(data, &ubo, sizeof(ubo));
    renderer->unmap_buffer(global_ubo);

    auto cmd = renderer->get_command_buffer(true);

    cmd->clear(0.3f, 0.5f, 0.3f, 1.0f);
    cmd->bind_pass("Swapchain_renderpass");
    cmd->bind_pipeline("triangle_pipeline");
    cmd->set_scissors(nullptr);
    cmd->set_viewport(nullptr);

    cmd->bind_descriptor_set(descriptor_set_handle);

    cmd->bind_descriptor_set(instance_descriptor_set_handle, 1);
    render_manager.render_all(cmd, Handle());

    renderer->end_frame();

    current_image++;
}

void RendererModule::resize_window(u32 width, u32 height)
{
    renderer->resize(width, height);
}
} // namespace modules
} // namespace sogas
