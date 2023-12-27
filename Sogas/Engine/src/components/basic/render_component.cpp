#include "pch.hpp"

#include <components/basic/render_component.h>
#include <engine/engine.h> // TODO Remove and use Resources module.
#include <handle/handle_manager.h>
#include <handle/object_manager.h>
#include <modules/render_manager.h>
#include <resources/mesh.h>

namespace sogas
{
using namespace components;
DECLARE_OBJECT_MANAGER("render", RenderComponent);
namespace components
{
bool RenderComponent::DrawCall::load(const json& j)
{
    if (!j["mesh"].is_string())
    {
        PERROR("Mesh defined in json is not a string.");
    }

    // TODO Should not be asked to the engine. Make Resources class.
    auto mesh_filename = j["mesh"].get<std::string>();
    resources::load_mesh(mesh_filename, mesh_filename);
    mesh    = Engine::Get().get_meshes()->at(mesh_filename);
    enabled = j.value("enabled", enabled);
    return true;
}

void RenderComponent::load(const json& j, EntityParser& /*context*/)
{
    if (j.is_array())
    {
        for (auto& jitem : j.items())
        {
            const json& jentry = jitem.value();
            DrawCall    dc;
            if (dc.load(jentry))
            {
                draw_calls.push_back(dc);
            }
        }
    }
}

void RenderComponent::update_render_manager()
{
    Handle h(this);
    for (auto& dc : draw_calls)
    {
        if (!dc.enabled)
        {
            continue;
        }

        modules::render_manager.add_key(h, dc.mesh);
    }
}

void RenderComponent::on_entity_created()
{
    update_render_manager();
}
} // namespace components
} // namespace sogas
