#pragma once

#include <components/base_component.h>

namespace sogas
{
namespace resources
{
class Mesh;
}
namespace components
{
class RenderComponent : public BaseComponent
{
    struct DrawCall
    {
        const resources::Mesh* mesh = nullptr;
        // TODO Material
        bool enabled{true};
        bool load(const json& j);
    };

  public:
    void render();
    void on_entity_created();

    void load(const json& j, EntityParser& context);

    void update_render_manager();

  private:
    std::vector<DrawCall> draw_calls;
};
} // namespace components
} // namespace sogas
