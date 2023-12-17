#pragma once

#include <resources/resources.h>

#pragma warning(disable : 4201)
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/hash.hpp>
#pragma warning(default : 4201)

namespace pinut
{
namespace resources
{
class CommandBuffer;
}
} // namespace pinut
namespace sogas
{
namespace resources
{
struct Vertex
{
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 normal   = glm::vec3(0.0f);
    glm::vec3 color    = glm::vec3(1.0f);
    glm::vec2 uv       = glm::vec2(0.0f);

    bool operator==(const Vertex& other) const
    {
        return position == other.position && normal == other.normal && color == other.color &&
               uv == other.uv;
    }
};

class Mesh
{
  public:
    ~Mesh()
    {
        destroy();
    }
    void draw(pinut::resources::CommandBuffer* cmd);
    void draw_indexed(pinut::resources::CommandBuffer* cmd);
    void destroy();

    std::vector<Vertex>                   vertices;
    std::vector<u16>                      indices;
    pinut::resources::BufferHandle        vertex_buffer{pinut::resources::invalid_buffer};
    pinut::resources::BufferHandle        index_buffer{pinut::resources::invalid_buffer};
    pinut::resources::DescriptorSetHandle descriptor_set;
};

void init_default_meshes();

void load_mesh(const std::string& name, const std::string& filename);

} // namespace resources
} // namespace sogas

namespace std
{
using namespace sogas::resources;
template <>
struct hash<Vertex>
{
    size_t operator()(Vertex const& vertex) const
    {
        return ((((hash<glm::vec3>()(vertex.position) ^ (hash<glm::vec3>()(vertex.normal) << 1)) >>
                  1) ^
                 (hash<glm::vec3>()(vertex.color) << 1)) >>
                1) ^
               (hash<glm::vec2>()(vertex.uv) << 1);
    }
};
} // namespace std
