#pragma once

namespace sogas
{
class Camera
{
  public:
    // clang-format off
    const glm::mat4& get_view() { return view; }
    const glm::mat4& get_projection() { return projection; }
    const glm::mat4& get_view_projection() { return view_projection; }

    glm::vec3 get_forward() const { return forward; }
    glm::vec3 get_up() const { return up; }
    glm::vec3 get_right() const { return -left; }
    glm::vec3 get_eye() const { return eye; }

    f32 get_aspect_ratio() const { return aspect_ratio; }
    f32 get_degrees_fov() const { return glm::degrees(fov); }
    f32 get_radians_fov() const { return fov; }
    f32 get_near() const { return z_min; }
    f32 get_far() const { return z_max; }
    // clang-format on

    void look_at(glm::vec3 new_eye,
                 glm::vec3 new_target,
                 glm::vec3 new_up = glm::vec3(0.0f, 1.0f, 0.0f));
    void set_projection_parameters(f32 new_rad_fov,
                                   f32 new_aspect_ratio,
                                   f32 new_near,
                                   f32 new_far);
    void set_orthographic_parameters(bool is_centered,
                                     f32  left,
                                     f32  width,
                                     f32  up,
                                     f32  height,
                                     f32  new_near,
                                     f32  new_far);

  protected:
    void update_view_projection();

    glm::mat4 view            = glm::mat4(1.0f);
    glm::mat4 projection      = glm::mat4(1.0f);
    glm::mat4 view_projection = glm::mat4(1.0f);

    glm::vec3 eye     = glm::vec3(0.0f);
    glm::vec3 target  = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 forward = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 up      = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 left    = glm::vec3(-1.0f, 0.0f, 0.0f);

    f32 fov          = glm::radians(60.0f);
    f32 aspect_ratio = 1.0f;
    f32 z_min        = 0.01f;
    f32 z_max        = 1000.0f;

    bool is_ortho = false;
};
} // namespace sogas
