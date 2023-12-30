#pragma once

namespace sogas
{
class Transform
{
  public:
    void lookAt(glm::vec3 eye, glm::vec3 target, glm::vec3 up = glm::vec3(0, 1, 0));
    void set_euler_angles(f32 yaw, f32 pitch, f32 roll);
    void get_euler_angles(f32* yaw, f32* pitch, f32* roll = nullptr);

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

  protected:
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 scale    = glm::vec3(1.0f);
    glm::quat rotation = glm::quat();
};
} // namespace sogas
