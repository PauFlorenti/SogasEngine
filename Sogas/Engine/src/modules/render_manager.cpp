#include "pch.hpp"

#include <components/basic/transform_component.h>
#include <entity/entity.h>
#include <handle/handle_manager.h>
#include <handle/object_manager.h>
#include <modules/render_manager.h>
#include <render_device.h>
#include <resources/mesh.h>

namespace sogas
{
namespace modules
{
RenderManager render_manager;

void RenderManager::add_key(Handle owner, const Mesh* mesh)
{
    RenderKey key;
    key.owner_handle = owner;
    key.mesh         = mesh;

    keys.push_back(key);
}

void RenderManager::render_all(pinut::resources::CommandBuffer* cmd, Handle /*camera_handle*/)
{
    /*Entity* camera = camera_handle;
    ASSERT(camera);*/

    for (auto& key : keys)
    {
        //Entity*                        entity = key.owner_handle;
        // components::TransformComponent transform_component =
        //   entity->get<components::TransformComponent>();

        key.mesh->draw_indexed(cmd);
    }
}
} // namespace modules
} // namespace sogas
