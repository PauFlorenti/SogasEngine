#include "pch.hpp"

#include <modules/module_renderer.h>
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

// TODO remove. Renderer should not calculate dt.
#include <chrono>
// TODO engine and camera are temporal
#include <engine/camera.h>
#include <engine/engine.h>

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
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

using namespace pinut::resources;

BufferHandle              global_ubo;
TextureHandle             texture_handle;
DescriptorSetHandle       descriptor_set_handle;
DescriptorSetLayoutHandle descriptor_set_layout_handle;

bool RendererModule::start()
{
    PINFO("Starting renderer module.");

    pinut::DeviceDescriptor descriptor;
    descriptor.set_window(1280, 720, window_handle);

    renderer = pinut::GPUDevice::create(pinut::GraphicsAPI::Vulkan);
    renderer->init(descriptor);

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

    pipeline_descriptor.vertex_input.add_vertex_stream({0, sizeof(sogas::resources::Vertex)});
    pipeline_descriptor.vertex_input.add_vertex_attribute(
      {0, 0, offsetof(sogas::resources::Vertex, position), VertexInputFormatType::VEC3});
    pipeline_descriptor.vertex_input.add_vertex_attribute(
      {1, 0, offsetof(sogas::resources::Vertex, color), VertexInputFormatType::VEC3});
    pipeline_descriptor.vertex_input.add_vertex_attribute(
      {2, 0, offsetof(sogas::resources::Vertex, uv), VertexInputFormatType::VEC2});

    // CREATING UNIFORM BUFFERS
    BufferDescriptor uniform_buffer_descriptor;
    uniform_buffer_descriptor.type = BufferType::UNIFORM;
    uniform_buffer_descriptor.size = sizeof(UniformBuffer);
    global_ubo                     = renderer->create_buffer(uniform_buffer_descriptor);

    // CREATING TEXTURE DESCRIPTOR
    u32               texture_data = 0xFF00FFFF;
    TextureDescriptor texture_descriptor{};
    texture_descriptor.width         = 1;
    texture_descriptor.height        = 1;
    texture_descriptor.channel_count = 4;
    texture_descriptor.data          = &texture_data;

    texture_handle = renderer->create_texture(texture_descriptor);

    // CREATING DESCRIPTOR SET LAYOUTS
    DescriptorSetBindingDescriptor binding = {0,
                                              1,
                                              ShaderStageType::VERTEX,
                                              DescriptorType::UNIFORM};

    DescriptorSetBindingDescriptor texture_binding = {1,
                                                      1,
                                                      ShaderStageType::FRAGMENT,
                                                      DescriptorType::COMBINED_IMAGE_SAMPLER};

    DescriptorSetLayoutDescriptor descriptor_set_layout_descriptor = {};
    descriptor_set_layout_descriptor.add_binding(binding)
      .add_binding(texture_binding)
      .add_name("GLOBAL");

    descriptor_set_layout_handle =
      renderer->create_descriptor_set_layout(descriptor_set_layout_descriptor);
    pipeline_descriptor.add_descriptor_set_layout(descriptor_set_layout_handle);

    DescriptorSetDescriptor descriptor_set_descriptor = {};
    descriptor_set_descriptor.set_layout(descriptor_set_layout_handle)
      .add_buffer(global_ubo, 0)
      .add_texture(texture_handle, 1);

    descriptor_set_handle = renderer->create_descriptor_set(descriptor_set_descriptor);

    // TODO: Handle pipeline creation differently ...
    // Data should be given from engine, not hardcoded in renderer.
    renderer->create_pipeline(pipeline_descriptor);

    return true;
}

void RendererModule::stop()
{
    PINFO("Shutting down renderer.");

    // TODO this should set up a queue, so that it can be destroyed once the gpu has finished working.
    renderer->destroy_descriptor_set_layout(descriptor_set_layout_handle);
    renderer->destroy_descriptor_set(descriptor_set_handle);
    renderer->destroy_texture(texture_handle);
    renderer->destroy_buffer(global_ubo);

    renderer->shutdown();
}

void RendererModule::render()
{
    renderer->begin_frame();

    static auto start_time    = std::chrono::high_resolution_clock::now();
    static u32  current_image = 0;

    auto current_time = std::chrono::high_resolution_clock::now();
    f32  dt =
      std::chrono::duration<f32, std::chrono::seconds::period>(current_time - start_time).count();

    UniformBuffer ubo{};

    auto camera = sogas::engine::Engine::Get().get_camera();
    ubo.view    = camera.get_view();
    ubo.proj    = camera.get_projection();
    ubo.proj[1][1] *= -1;

    ubo.model =
      glm::rotate(glm::mat4(1.0f), dt * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) *
      glm::rotate(glm::mat4(1.0f), dt * glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

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

    auto meshes = sogas::engine::Engine::Get().get_meshes();

    auto it = meshes->find("cube");
    if (it != meshes->end())
    {
        it->second->draw_indexed(cmd);
    }

    renderer->end_frame();

    current_image++;
}

void RendererModule::resize_window(u32 width, u32 height)
{
    renderer->resize(width, height);
}
} // namespace modules
} // namespace sogas
