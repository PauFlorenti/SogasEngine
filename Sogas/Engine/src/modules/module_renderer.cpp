#include <fstream>
#include <modules/module_renderer.h>
#include <resources/pipeline.h>
#include <resources/shader_state.h>

#include <chrono>

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
};

struct UniformBuffer
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

std::vector<Vertex> triangle = {{glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)},
                                {glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
                                {glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
                                {glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.5f, 0.7f)}};

std::vector<u16> indices = {0, 1, 2, 0, 2, 3};

u32 triangle_mesh;
u32 triangle_index;
u32 global_ubo;
u32 descriptor_set_id;
//u32 uniform_buffers[3];

bool RendererModule::start()
{
    pinut::DeviceDescriptor descriptor;
    descriptor.set_window(1280, 720, window_handle);

    renderer = pinut::GPUDevice::create(pinut::GraphicsAPI::Vulkan);
    renderer->init(descriptor);

    using namespace pinut::resources;

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

    PipelineDescriptor pipeline_descriptor = {};
    pipeline_descriptor.add_name("triangle_pipeline")
      .set_topology(TopologyType::TRIANGLE)
      .add_shader_state(shader_state)
      .add_viewport(viewport_state);

    pipeline_descriptor.vertex_input.add_vertex_stream({0, sizeof(Vertex)});
    pipeline_descriptor.vertex_input.add_vertex_attribute(
      {0, 0, offsetof(Vertex, position), VertexInputFormatType::VEC3});
    pipeline_descriptor.vertex_input.add_vertex_attribute(
      {1, 0, offsetof(Vertex, color), VertexInputFormatType::VEC3});

    // CREATING VERTEX AND INDEX BUFFER
    u32 buffer_size = static_cast<u32>(sizeof(Vertex) * triangle.size());

    BufferDescriptor buffer_descriptor;
    buffer_descriptor.size = buffer_size;
    triangle_mesh          = renderer->create_buffer(buffer_descriptor).id;

    BufferDescriptor staging_buffer_descriptor;
    staging_buffer_descriptor.size = buffer_size;
    staging_buffer_descriptor.type = BufferType::STAGING;
    staging_buffer_descriptor.data = triangle.data();
    const auto staging_buffer      = renderer->create_buffer(staging_buffer_descriptor).id;

    renderer->copy_buffer(staging_buffer, triangle_mesh, buffer_size);
    renderer->destroy_buffer({staging_buffer});

    const auto index_buffer_size = static_cast<u32>(sizeof(u32) * indices.size());
    buffer_descriptor.size       = index_buffer_size;
    buffer_descriptor.type       = BufferType::INDEX;
    triangle_index               = renderer->create_buffer(buffer_descriptor).id;

    BufferDescriptor index_staging_buffer_descriptor;
    index_staging_buffer_descriptor.data = indices.data();
    index_staging_buffer_descriptor.size = index_buffer_size;
    index_staging_buffer_descriptor.type = BufferType::STAGING;
    const auto index_staging_buffer = renderer->create_buffer(index_staging_buffer_descriptor).id;

    renderer->copy_buffer(index_staging_buffer, triangle_index, index_buffer_size);
    renderer->destroy_buffer({index_staging_buffer});

    // CREATING UNIFORM BUFFERS
    BufferDescriptor uniform_buffer_descriptor;
    uniform_buffer_descriptor.type = BufferType::UNIFORM;
    uniform_buffer_descriptor.size = sizeof(UniformBuffer);
    global_ubo                     = renderer->create_buffer(uniform_buffer_descriptor).id;

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

    renderer->create_pipeline(pipeline_descriptor);

    return true;
}

void RendererModule::stop()
{
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
    cmd->bind_vertex_buffer(triangle_mesh, 0, 0);
    cmd->bind_index_buffer(triangle_index);

    cmd->draw_indexed(0, static_cast<u32>(indices.size()), 0, 1, 0);

    renderer->end_frame();

    current_image++;
}
void RendererModule::resize_window(u32 width, u32 height)
{
    renderer->resize(width, height);
}
} // namespace modules
} // namespace sogas
