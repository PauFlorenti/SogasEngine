#include <fstream>
#include <modules/module_renderer.h>
#include <resources/pipeline.h>
#include <resources/shader_state.h>

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

struct vec3
{
    f32 x;
    f32 y;
    f32 z;
};

struct Vertex
{
    vec3 position;
    vec3 color;
};

std::vector<Vertex> triangle = {{vec3(0.5f, 0.5f, 0.0f), vec3(1.0f, 0.0f, 0.0f)},
                                {vec3(-0.5f, 0.5f, 0.0f), vec3(0.0f, 1.0f, 0.0f)},
                                {vec3(0.0f, -0.5f, 0.0f), vec3(0.0f, 0.0f, 1.0f)}};

u32 triangle_mesh;

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
      .add_shader_state(shader_state)
      .add_viewport(viewport_state);

    pipeline_descriptor.vertex_input.add_vertex_stream({0, sizeof(Vertex)});
    pipeline_descriptor.vertex_input.add_vertex_attribute({0, 0, offsetof(Vertex, position)});
    pipeline_descriptor.vertex_input.add_vertex_attribute({1, 0, offsetof(Vertex, color)});

    u32 buffer_size = static_cast<u32>(sizeof(Vertex) * triangle.size());

    BufferDescriptor buffer_descriptor;
    buffer_descriptor.size = buffer_size;
    triangle_mesh          = renderer->create_buffer(buffer_descriptor).id;

    {
        auto data = renderer->map_buffer(triangle_mesh, buffer_size);
        memcpy(data, triangle.data(), buffer_size);
        renderer->unmap_buffer(triangle_mesh);
    }

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

    auto cmd = renderer->get_command_buffer(true);

    cmd->clear(0.3f, 0.5f, 0.3f, 1.0f);
    cmd->bind_pass("Swapchain_renderpass");
    cmd->bind_pipeline("triangle_pipeline");
    cmd->set_scissors(nullptr);
    cmd->set_viewport(nullptr);

    cmd->bind_vertex_buffer(triangle_mesh, 0, 0);
    cmd->draw(0, 3, 0, 1);

    renderer->end_frame();
}
void RendererModule::resize_window(u32 width, u32 height)
{
    renderer->resize(width, height);
}
} // namespace modules
} // namespace sogas
