#include "pch.hpp"

#include <modules/module_renderer.h>
#include <resources/pipeline.h>
#include <resources/resources.h>
#include <resources/shader_state.h>

// TODO remove. Renderer should not calculate dt.
#include <chrono>
#include <stb_image.h>

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

struct Vertex
{
    glm::vec3 position;
    glm::vec3 color;
    //glm::vec3 normal;
};

struct UniformBuffer
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

std::vector<Vertex> plane_vertices = {{glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)},
                                      {glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
                                      {glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
                                      {glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.5f, 0.7f)}};

std::vector<u16> plane_indices = {0, 1, 2, 0, 2, 3};

// clang-format off
std::vector<Vertex> cube_vertices = {
    //Top
    {glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f)},  //0
    {glm::vec3( 0.5f, 0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f)},  //1
    {glm::vec3(-0.5f, 0.5f,  0.5f), glm::vec3(1.0f, 0.0f, 0.0f)},  //2
    {glm::vec3( 0.5f, 0.5f,  0.5f), glm::vec3(1.0f, 0.0f, 0.0f)},  //3

    //Bottom
    {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f)}, //4
    {glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f)}, //5
    {glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f, 1.0f, 0.0f)}, //6
    {glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(0.0f, 1.0f, 0.0f)}, //7

    //Front
    {glm::vec3(-0.5f,  0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f)}, //8
    {glm::vec3( 0.5f,  0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f)}, //9
    {glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f)}, //10
    {glm::vec3( 0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f)}, //11

    //Back
    {glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(1.0f, 0.5f, 0.0f)}, //12
    {glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3(1.0f, 0.5f, 0.0f)}, //13
    {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 0.5f, 0.0f)}, //14
    {glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 0.5f, 0.0f)}, //15

    //Left
    {glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.5, 1.0f, 0.5f)}, //16
    {glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.5, 1.0f, 0.5f)}, //17
    {glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.5, 1.0f, 0.5f)}, //18
    {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5, 1.0f, 0.5f)}, //19

    //Right
    {glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(0.5f, 0.5f, 1.0f)}, //20
    {glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 1.0f)}, //21
    {glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(0.5f, 0.5f, 1.0f)}, //22
    {glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 1.0f)}  //23
};

std::vector<u16> cube_indices = { 
    //Top
    0, 1, 2,
    2, 3, 1,

    //Bottom
    4, 5, 6,
    6, 7, 5,

    //Front
    8, 9, 10,
    10, 11, 9,

    //Back
    12, 13, 14,
    14, 15, 13,

    //Left
    16, 17, 18,
    18, 19, 17,

    //Right
    20, 21, 22,
    22, 23, 21 };
// clang-format on

pinut::resources::BufferHandle  plane_buffer_vertices;
pinut::resources::BufferHandle  plane_buffer_indices;
pinut::resources::BufferHandle  cube_buffer_vertices;
pinut::resources::BufferHandle  cube_buffer_indices;
pinut::resources::BufferHandle  global_ubo;
pinut::resources::TextureHandle texture_handle;
u32                             descriptor_set_id;

using namespace pinut::resources;

bool RendererModule::start()
{
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

    pipeline_descriptor.vertex_input.add_vertex_stream({0, sizeof(Vertex)});
    pipeline_descriptor.vertex_input.add_vertex_attribute(
      {0, 0, offsetof(Vertex, position), VertexInputFormatType::VEC3});
    pipeline_descriptor.vertex_input.add_vertex_attribute(
      {1, 0, offsetof(Vertex, color), VertexInputFormatType::VEC3});

    // CREATING VERTEX AND INDEX BUFFER
    // CUBE
    const auto cube_vertex_buffer_size = static_cast<u32>(sizeof(Vertex) * cube_vertices.size());

    BufferDescriptor cube_buffer_descriptor;
    cube_buffer_descriptor.size = cube_vertex_buffer_size;
    cube_buffer_vertices        = renderer->create_buffer(cube_buffer_descriptor);

    BufferDescriptor cube_staging_buffer_descriptor;
    cube_staging_buffer_descriptor.size = cube_vertex_buffer_size;
    cube_staging_buffer_descriptor.type = BufferType::STAGING;
    cube_staging_buffer_descriptor.data = cube_vertices.data();
    const auto cube_staging_buffer      = renderer->create_buffer(cube_staging_buffer_descriptor);

    renderer->copy_buffer(cube_staging_buffer, cube_buffer_vertices, cube_vertex_buffer_size);
    renderer->destroy_buffer({cube_staging_buffer});

    const auto cube_indices_buffer_size = static_cast<u32>(sizeof(u16) * cube_indices.size());
    cube_buffer_descriptor.size         = cube_indices_buffer_size;
    cube_buffer_descriptor.type         = BufferType::INDEX;
    cube_buffer_indices                 = renderer->create_buffer(cube_buffer_descriptor);

    BufferDescriptor index_staging_buffer_descriptor;
    index_staging_buffer_descriptor.data = cube_indices.data();
    index_staging_buffer_descriptor.size = cube_indices_buffer_size;
    index_staging_buffer_descriptor.type = BufferType::STAGING;
    const auto index_staging_buffer      = renderer->create_buffer(index_staging_buffer_descriptor);

    renderer->copy_buffer(index_staging_buffer, cube_buffer_indices, cube_indices_buffer_size);
    renderer->destroy_buffer({index_staging_buffer});

    // PLANE
    const auto plane_vertices_size = static_cast<u32>(sizeof(Vertex) * plane_vertices.size());

    BufferDescriptor plane_buffer_descriptor{};
    plane_buffer_descriptor.size = plane_vertices_size;
    plane_buffer_descriptor.type = BufferType::VERTEX;
    plane_buffer_vertices        = renderer->create_buffer(plane_buffer_descriptor);

    BufferDescriptor plane_staging_buffer_descriptor{};
    plane_staging_buffer_descriptor.data = plane_vertices.data();
    plane_staging_buffer_descriptor.size = plane_vertices_size;
    plane_staging_buffer_descriptor.type = BufferType::STAGING;

    auto plane_staging_buffer_vertices = renderer->create_buffer(plane_staging_buffer_descriptor);

    renderer->copy_buffer(plane_staging_buffer_vertices,
                          plane_buffer_vertices,
                          plane_vertices_size);

    renderer->destroy_buffer(plane_staging_buffer_vertices);

    const auto indices_buffer_size = static_cast<u32>(sizeof(u16) * plane_indices.size());

    BufferDescriptor plane_indices_buffer_descriptor{};
    plane_indices_buffer_descriptor.size = indices_buffer_size;
    plane_indices_buffer_descriptor.type = BufferType::INDEX;

    plane_buffer_indices = renderer->create_buffer(plane_indices_buffer_descriptor);

    BufferDescriptor plane_indices_staging_buffer_descriptor{};
    plane_indices_staging_buffer_descriptor.data = plane_indices.data();
    plane_indices_staging_buffer_descriptor.size = indices_buffer_size;
    plane_indices_staging_buffer_descriptor.type = BufferType::STAGING;

    auto plane_staging_buffer_indices =
      renderer->create_buffer(plane_indices_staging_buffer_descriptor);

    renderer->copy_buffer(plane_staging_buffer_indices, plane_buffer_indices, indices_buffer_size);

    renderer->destroy_buffer(plane_staging_buffer_indices);

    // CREATING UNIFORM BUFFERS
    BufferDescriptor uniform_buffer_descriptor;
    uniform_buffer_descriptor.type = BufferType::UNIFORM;
    uniform_buffer_descriptor.size = sizeof(UniformBuffer);
    global_ubo                     = renderer->create_buffer(uniform_buffer_descriptor);

    // CREATING DESCRIPTOR SET LAYOUTS
    DescriptorSetBindingDescriptor binding = {0,
                                              1,
                                              ShaderStageType::VERTEX,
                                              DescriptorType::UNIFORM};

    DescriptorSetLayoutDescriptor descriptor_set_layout_descriptor = {};
    descriptor_set_layout_descriptor.add_binding(&binding).add_name("GLOBAL");

    const auto descriptor_set_layout_id =
      renderer->create_descriptor_set_layout(descriptor_set_layout_descriptor);
    pipeline_descriptor.add_descriptor_set_layout(descriptor_set_layout_id);

    DescriptorSetDescriptor descriptor_set_descriptor = {};
    descriptor_set_descriptor.set_layout(descriptor_set_layout_id).add_buffer({global_ubo}, 0);

    descriptor_set_id = renderer->create_descriptor_set(descriptor_set_descriptor);

    // TODO: Handle pipeline creation differently ...
    // Data should be given from engine, not hardcoded in renderer.

    u32               texture_data = 0xFFFFFFFF;
    TextureDescriptor texture_descriptor{};
    texture_descriptor.width         = 1;
    texture_descriptor.height        = 1;
    texture_descriptor.channel_count = 4;
    texture_descriptor.data          = &texture_data;

    texture_handle = renderer->create_texture(texture_descriptor);

    renderer->create_pipeline(pipeline_descriptor);

    return true;
}

void RendererModule::stop()
{
    renderer->destroy_texture(texture_handle);
    renderer->destroy_buffer(global_ubo);
    renderer->destroy_buffer(plane_buffer_indices);
    renderer->destroy_buffer(plane_buffer_vertices);
    renderer->destroy_buffer(cube_buffer_indices);
    renderer->destroy_buffer(cube_buffer_vertices);

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
    ubo.model =
      glm::rotate(glm::mat4(1.0f), dt * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) *
      glm::rotate(glm::mat4(1.0f), dt * glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f),
                           glm::vec3(0.0f, 0.0f, 0.0f),
                           glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), 1280.0f / (float)720.0f, 0.1f, 10.0f);
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

    cmd->bind_descriptor_set(descriptor_set_id);
    cmd->bind_vertex_buffer(cube_buffer_vertices, 0, 0);
    cmd->bind_index_buffer(cube_buffer_indices, BufferIndexType::UINT16);

    cmd->draw_indexed(0, static_cast<u32>(cube_indices.size()), 0, 1, 0);

    cmd->bind_vertex_buffer(plane_buffer_vertices, 0, 0);
    cmd->bind_index_buffer(plane_buffer_indices, BufferIndexType::UINT16);
    cmd->draw_indexed(0, static_cast<u32>(plane_indices.size()), 0, 1, 0);

    renderer->end_frame();

    current_image++;
}

void RendererModule::resize_window(u32 width, u32 height)
{
    renderer->resize(width, height);
}
} // namespace modules
} // namespace sogas
