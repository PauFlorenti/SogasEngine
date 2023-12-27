#include <engine/engine.h>
#include <modules/module_renderer.h>
#include <render_device.h>
#include <resources/mesh.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tinyobj/tiny_obj_loader.h"

namespace sogas
{
// clang-format off
    std::vector<Vertex> plane_vertices = {
      {glm::vec3( 0.5f, 0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f), glm::vec2(0.0f, 0.0f)},
      {glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f), glm::vec2(1.0f, 1.0f)},
      {glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f), glm::vec2(1.0f, 0.0f)},
      {glm::vec3( 0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f), glm::vec2(0.0f, 0.0f)},
    };

    std::vector<u16> plane_indices = { 0, 1, 2, 0, 2, 3 };

    std::vector<Vertex> cube_vertices = {
        //Top
        {glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec2(0.0f, 0.0f)},  //0
        {glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec2(1.0f, 0.0f)},  //1
        {glm::vec3(-0.5f, 0.5f,  0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec2(1.0f, 1.0f)},  //2
        {glm::vec3(0.5f, 0.5f,  0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec2(0.0f, 1.0f)},  //3

        //Bottom
        { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f),glm::vec3(0.0f), glm::vec2(1.0f, 0.0f) }, //4
        { glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f),glm::vec2(1.0f, 1.0f) }, //5
        { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f, 1.0f, 0.0f),glm::vec3(0.0f), glm::vec2(0.0f, 0.0f) }, //6
        { glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f),glm::vec2(0.0f, 1.0f)}, //7

        //Front
        { glm::vec3(-0.5f,  0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f),glm::vec3(0.0f), glm::vec2(0.0f, 0.0f) }, //8
        { glm::vec3(0.5f,  0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f),glm::vec2(0.0f, 1.0f) }, //9
        { glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f),glm::vec3(0.0f), glm::vec2(1.0f, 0.0f) }, //10
        { glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f),glm::vec2(1.0f, 1.0f)}, //11

        //Back
        { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(1.0f, 0.5f, 0.0f),glm::vec3(0.0f), glm::vec2(0.0f, 0.0f) }, //12
        { glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(1.0f, 0.5f, 0.0f), glm::vec3(0.0f),glm::vec2(0.0f, 1.0f) }, //13
        { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 0.5f, 0.0f),glm::vec3(0.0f), glm::vec2(1.0f, 0.0f) }, //14
        { glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 0.5f, 0.0f), glm::vec3(0.0f),glm::vec2(1.0f, 1.0f)}, //15

        //Left
        { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.5, 1.0f, 0.5f), glm::vec3(0.0f), glm::vec2(0.0f, 0.0f) }, //16
        { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.5, 1.0f, 0.5f), glm::vec3(0.0f), glm::vec2(0.0f, 1.0f) }, //17
        { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.5, 1.0f, 0.5f), glm::vec3(0.0f), glm::vec2(1.0f, 0.0f) }, //18
        { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5, 1.0f, 0.5f), glm::vec3(0.0f), glm::vec2(1.0f, 1.0f)}, //19

        //Right
        { glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(0.5f, 0.5f, 1.0f), glm::vec3(0.0f), glm::vec2(0.0f, 0.0f) }, //20
        { glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 1.0f), glm::vec3(0.0f), glm::vec2(0.0f, 1.0f) }, //21
        { glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(0.5f, 0.5f, 1.0f), glm::vec3(0.0f), glm::vec2(1.0f, 0.0f) }, //22
        { glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 1.0f), glm::vec3(0.0f), glm::vec2(1.0f, 1.0f)}  //23
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

static void upload_buffer(pinut::resources::BufferHandle buffer, const u32 size, void* data)
{
    auto device = sogas::Engine::Get().get_renderer()->get_device();

    const auto vertex_staging_buffer =
      device->create_buffer({size, pinut::resources::BufferType::STAGING, data});

    device->copy_buffer(vertex_staging_buffer, buffer, size);
    device->destroy_buffer({vertex_staging_buffer});
}

void init_default_meshes()
{
    auto device = sogas::Engine::Get().get_renderer()->get_device();
    auto meshes = sogas::Engine::Get().get_meshes();

    Mesh*     plane             = new Mesh();
    const u32 plane_vertex_size = static_cast<u32>(plane_vertices.size() * sizeof(Vertex));
    const u32 plane_index_size  = static_cast<u32>(plane_indices.size() * sizeof(u16));

    plane->vertices      = plane_vertices;
    plane->indices       = plane_indices;
    plane->vertex_buffer = device->create_buffer({plane_vertex_size});
    plane->index_buffer =
      device->create_buffer({plane_index_size, pinut::resources::BufferType::INDEX});

    upload_buffer(plane->vertex_buffer, plane_vertex_size, plane_vertices.data());
    upload_buffer(plane->index_buffer, plane_index_size, plane_indices.data());

    meshes->insert({"plane", plane});

    Mesh*     cube             = new Mesh();
    const u32 cube_vertex_size = static_cast<u32>(cube_vertices.size() * sizeof(Vertex));
    const u32 cube_index_size  = static_cast<u32>(cube_indices.size() * sizeof(u16));

    cube->vertices      = cube_vertices;
    cube->indices       = cube_indices;
    cube->vertex_buffer = device->create_buffer({cube_vertex_size});
    cube->index_buffer =
      device->create_buffer({cube_index_size, pinut::resources::BufferType::INDEX});

    upload_buffer(cube->vertex_buffer, cube_vertex_size, cube_vertices.data());
    upload_buffer(cube->index_buffer, cube_index_size, cube_indices.data());
    meshes->insert({"cube", cube});
}

void load_mesh(const std::string& name, const std::string& filename)
{
    auto meshes = sogas::Engine::Get().get_meshes();

    if (auto mesh = meshes->find(filename); mesh != meshes->end())
    {
        return;
    }

    // Not found, try to load resource.
    std::ifstream file(filename.c_str());

    if (file.fail())
    {
        throw std::runtime_error("Failed to open .obj file.");
    }

    tinyobj::attrib_t attrib;

    std::vector<tinyobj::shape_t>    shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warn, err;

    tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename.c_str());

    if (!warn.empty())
    {
        PWARN(warn.c_str());
    }

    if (!err.empty())
    {
        PERROR(err.c_str());
        ASSERT(false);
    }

    Mesh*                                mesh = new Mesh();
    std::unordered_map<Vertex, uint32_t> uniqueVertices{};

    for (const auto& shape : shapes)
    {
        for (const auto& index : shape.mesh.indices)
        {
            Vertex vertex{};

            vertex.position = {attrib.vertices[3 * index.vertex_index + 0],
                               attrib.vertices[3 * index.vertex_index + 1],
                               attrib.vertices[3 * index.vertex_index + 2]};

            if (!attrib.normals.empty())
            {
                vertex.normal = {attrib.normals[3 * index.normal_index + 0],
                                 attrib.normals[3 * index.normal_index + 1],
                                 attrib.normals[3 * index.normal_index + 2]};
            }

            vertex.color = {1.0f, 1.0f, 1.0f};

            if (attrib.texcoords.size() > 0)
            {
                vertex.uv = {attrib.texcoords[2 * index.texcoord_index + 0],
                             1.0f - attrib.texcoords[2 * index.texcoord_index + 1]};
            }

            if (uniqueVertices.count(vertex) == 0)
            {
                uniqueVertices[vertex] = static_cast<uint32_t>(mesh->vertices.size());
                mesh->vertices.push_back(vertex);
            }

            mesh->indices.push_back(static_cast<u16>(uniqueVertices[vertex]));
        }
    }

    auto device = sogas::Engine::Get().get_renderer()->get_device();

    const u32 vertex_buffer_size = static_cast<u32>(mesh->vertices.size() * sizeof(Vertex));
    const u32 index_buffer_size  = static_cast<u32>(mesh->indices.size() * sizeof(u16));

    const auto vertex_staging_buffer = device->create_buffer(
      {vertex_buffer_size, pinut::resources::BufferType::STAGING, mesh->vertices.data()});
    mesh->vertex_buffer = device->create_buffer({vertex_buffer_size});

    device->copy_buffer(vertex_staging_buffer, mesh->vertex_buffer, vertex_buffer_size);
    device->destroy_buffer({vertex_staging_buffer});

    const auto index_staging_buffer = device->create_buffer(
      {index_buffer_size, pinut::resources::BufferType::STAGING, mesh->indices.data()});
    mesh->index_buffer =
      device->create_buffer({index_buffer_size, pinut::resources::BufferType::INDEX});

    device->copy_buffer(index_staging_buffer, mesh->index_buffer, index_buffer_size);
    device->destroy_buffer({index_staging_buffer});

    meshes->insert({name, mesh});
}

void Mesh::draw(pinut::resources::CommandBuffer* cmd) const
{
    cmd->bind_vertex_buffer(vertex_buffer, 0, 0);
    cmd->draw(0, static_cast<u32>(vertices.size()), 0, 1);
}

void Mesh::draw_indexed(pinut::resources::CommandBuffer* cmd) const
{
    cmd->bind_vertex_buffer(vertex_buffer, 0, 0);
    cmd->bind_index_buffer(index_buffer, pinut::resources::BufferIndexType::UINT16);
    cmd->draw_indexed(0, static_cast<u32>(indices.size()), 0, 1, 0);
}
void Mesh::destroy()
{
    auto device = sogas::Engine::Get().get_renderer()->get_device();
    vertices.clear();
    indices.clear();

    device->destroy_buffer(vertex_buffer);
    device->destroy_buffer(index_buffer);
}
} // namespace sogas
