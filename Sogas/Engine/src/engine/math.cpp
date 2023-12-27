#include "pch.hpp"

namespace sogas
{
glm::vec3 yaw_to_vector(f32 yaw)
{
    return glm::vec3(sinf(yaw), 0.0f, cosf(yaw));
}

f32 vector_to_yaw(glm::vec3 vector)
{
    return atan2f(vector.x, vector.z);
}

glm::vec3 yaw_pitch_to_vector(f32 yaw, f32 pitch)
{
    return glm::vec3(sinf(yaw) * cosf(-pitch), sinf(-pitch), cosf(yaw) * cosf(-pitch));
}

void vector_to_yaw_pitch(glm::vec3 vector, f32* yaw, f32* pitch)
{
    *yaw      = vector_to_yaw(vector);
    f32 modul = sqrtf(vector.x * vector.x + vector.z * vector.z);
    *pitch    = atan2(-vector.y, modul);
}

} // namespace sogas
