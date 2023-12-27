#pragma once

#include <components/base_component.h>

namespace sogas
{
namespace components
{
// TODO Maybe a transform class with all this should be created?
// We may want to have a transform that is not necessary a component.
class TransformComponent : public BaseComponent
{
  public:
    void update(f32 /*delta_time*/){};
    void on_entity_created(){};
    void load(const nlohmann::json& j, EntityParser& context);

    void lookAt(glm::vec3 pos, glm::vec3 target, glm::vec3 up);
    void set_euler_angles(f32 yaw, f32 pitch, f32 roll);

    // clang-format off
    void set_rotation(glm::quat new_rotation) { rotation = new_rotation; }
    void set_position(glm::vec3 new_position) { position = new_position; }
    void set_scale(glm::vec3 new_scale) { scale = new_scale; }

    glm::quat get_rotation() const { return rotation; }
    glm::vec3 get_position() const { return position; }
    glm::vec3 get_scale() const { return scale; }
    // clang-format on

    glm::vec3 get_forward() const;
    glm::vec3 get_right() const;
    glm::vec3 get_up() const;

    glm::mat4 as_matrix() const;
    void      from_matrix(glm::mat4 matrix);

  private:
    glm::vec3 position;
    glm::vec3 scale;
    glm::quat rotation;
};
} // namespace components
} // namespace sogas
