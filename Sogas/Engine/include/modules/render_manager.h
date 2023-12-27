#pragma once

namespace pinut::resources
{
class CommandBuffer;
}

namespace sogas
{
namespace resources
{
class Mesh;
}
namespace modules
{
class RenderManager
{
    struct RenderKey
    {
        Handle                 owner_handle;
        const resources::Mesh* mesh = nullptr;
        // TODO material.
        Handle transform;
    };

  public:
    void add_key(Handle owner, const resources::Mesh* mesh);
    void render_all(pinut::resources::CommandBuffer* cmd, Handle camera_handle);

  private:
    std::vector<RenderKey> keys;
};

extern RenderManager render_manager;
} // namespace modules
} // namespace sogas
