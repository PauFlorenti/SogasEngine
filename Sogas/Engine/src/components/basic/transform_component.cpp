#include "pch.hpp"

#include <components/basic/transform_component.h>
#include <handle/handle_manager.h>
#include <handle/object_manager.h>
#include <resources/json_helper.h>

namespace sogas
{
DECLARE_OBJECT_MANAGER("transform", TransformComponent);
void TransformComponent::load(const json& j, EntityParser& /*context*/)
{
    if (j.count("pos"))
    {
        position = load_vec3(j, "pos");
    }

    if (j.count("lookat"))
    {
        lookAt(get_position(), load_vec3(j, "lookat"), glm::vec3(0.0f, 1.0f, 0.0f));
    }

    if (j.count("rot"))
    {
        rotation = load_quat(j, "rot");
    }

    if (j.count("euler"))
    {
        glm::vec3 euler = load_vec3(j, "euler");

        euler.x = glm::radians(euler.x);
        euler.y = glm::radians(euler.y);
        euler.z = glm::radians(euler.z);

        set_euler_angles(euler.x, euler.y, euler.z);
    }

    if (j.count("scale"))
    {
        const json& jscale = j["scale"];

        if (jscale.is_number())
        {
            scale = glm::vec3(jscale.get<f32>());
        }
        else
        {
            scale = load_vec3(j, "scale");
        }
    }
}

void TransformComponent::lookAt(glm::vec3 pos, glm::vec3 target, glm::vec3 /*up*/)
{
    position          = pos;
    glm::vec3 forward = target - position;
    f32       yaw, pitch;
    vector_to_yaw_pitch(forward, &yaw, &pitch);
    set_euler_angles(yaw, pitch, 0.0f);
}

void TransformComponent::set_euler_angles(f32 yaw, f32 pitch, f32 roll)
{
    rotation = glm::quat(glm::vec3(pitch, yaw, roll));
}

void TransformComponent::get_euler_angles(f32* yaw, f32* pitch, f32* roll)
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

glm::vec3 TransformComponent::get_forward() const
{
    return glm::normalize(as_matrix()[2]);
}

glm::vec3 TransformComponent::get_right() const
{
    return glm::normalize(as_matrix()[0]);
}

glm::vec3 TransformComponent::get_up() const
{
    return glm::normalize(as_matrix()[1]);
}

glm::mat4 TransformComponent::as_matrix() const
{
    return glm::translate(glm::mat4(1.0f), position) * glm::mat4_cast(rotation) *
           glm::scale(glm::mat4(1.0f), scale);
}

void TransformComponent::from_matrix(glm::mat4 matrix)
{
    glm::vec3 auxiliar;
    glm::vec4 perspective;
    glm::decompose(matrix, scale, rotation, position, auxiliar, perspective);
}
} // namespace sogas
