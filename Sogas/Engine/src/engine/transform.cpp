#include "pch.hpp"

#include <engine/transform.h>

namespace sogas
{

void Transform::lookAt(glm::vec3 eye, glm::vec3 target, glm::vec3 /*up*/)
{
    position          = eye;
    glm::vec3 forward = target - position;
    f32       yaw, pitch;
    vector_to_yaw_pitch(forward, &yaw, &pitch);
    set_euler_angles(yaw, pitch, 0.0f);
}

// Set euler angles in degrees.
void Transform::set_euler_angles(f32 yaw, f32 pitch, f32 roll)
{
    rotation = glm::quat({glm::radians(pitch), glm::radians(yaw), glm::radians(roll)});
}

void Transform::get_euler_angles(f32* yaw, f32* pitch, f32* roll)
{
    const auto forward = get_forward();
    vector_to_yaw_pitch(forward, yaw, pitch);

    if (roll)
    {
        const auto roll_zero_left = glm::normalize(glm::cross(glm::vec3(0, 1, 0), forward));
        const auto roll_zero_up   = glm::normalize(glm::cross(forward, roll_zero_left));
        const auto current_left   = -glm::normalize(get_right());

        *roll =
          atan2f(glm::dot(current_left, roll_zero_up), glm::dot(current_left, roll_zero_left));
    }
}

glm::vec3 Transform::get_forward() const
{
    return glm::normalize(as_matrix()[2]);
}

glm::vec3 Transform::get_right() const
{
    return glm::normalize(as_matrix()[0]);
}

glm::vec3 Transform::get_up() const
{
    return glm::normalize(as_matrix()[1]);
}

glm::mat4 Transform::as_matrix() const
{
    return glm::translate(glm::mat4(1.0f), position) * glm::mat4_cast(rotation) *
           glm::scale(glm::mat4(1.0f), scale);
}

void Transform::from_matrix(glm::mat4 matrix)
{
    glm::vec3 auxiliar;
    glm::vec4 perspective;
    glm::decompose(matrix, scale, rotation, position, auxiliar, perspective);
}
} // namespace sogas
