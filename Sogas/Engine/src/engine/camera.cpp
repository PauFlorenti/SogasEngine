#include <engine/camera.h>

namespace sogas
{
void Camera::look_at(glm::vec3 new_eye, glm::vec3 new_target, glm::vec3 new_up)
{
    eye    = new_eye;
    target = new_target;

    view = glm::lookAt(eye, target, new_up);
    update_view_projection();

    forward = glm::normalize(target - eye);
    left    = glm::normalize(glm::cross(new_up, forward));
    up      = glm::normalize(glm::cross(forward, left));
}

void Camera::set_projection_parameters(f32 new_rad_fov,
                                       f32 new_aspect_ratio,
                                       f32 new_near,
                                       f32 new_far)
{
    fov          = new_rad_fov;
    aspect_ratio = new_aspect_ratio;
    z_min        = new_near;
    z_max        = new_far;
    is_ortho     = false;

    projection = glm::perspective(fov, aspect_ratio, z_min, z_max);

    update_view_projection();
}

void Camera::set_orthographic_parameters(bool /*is_centered*/,
                                         f32 new_left,
                                         f32 width,
                                         f32 new_top,
                                         f32 height,
                                         f32 new_near,
                                         f32 new_far)
{
    // TODO At the moment assume it is always centered.
    z_min    = new_near;
    z_max    = new_far;
    is_ortho = true;

    projection = glm::ortho(new_left, width, height, new_top);

    update_view_projection();
}

void Camera::update_view_projection()
{
    view_projection = view * projection;
}
} // namespace sogas
