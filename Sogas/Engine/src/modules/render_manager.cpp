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
    key.transform    = Handle();

    keys.push_back(key);
}

void RenderManager::render_all(pinut::resources::CommandBuffer* cmd, Handle /*camera_handle*/)
{
    // TODO If keys are dirty, sort keys.

    /*Entity* camera = camera_handle;
    ASSERT(camera);*/

    for (auto& key : keys)
    {
        Entity*             entity    = key.owner_handle.get_owner();
        TransformComponent* transform = entity->get<TransformComponent>();
        auto                model     = transform->as_matrix();

        cmd->set_push_constant(pinut::resources::ShaderStageType::VERTEX,
                               sizeof(glm::mat4),
                               0,
                               &model);
        key.mesh->draw_indexed(cmd);
    }
}
} // namespace modules
} // namespace sogas
