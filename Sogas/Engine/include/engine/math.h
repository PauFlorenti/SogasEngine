#pragma once

namespace sogas
{
glm::vec3 yaw_to_vector(f32 yaw);
f32       vector_to_yaw(glm::vec3 vector);
glm::vec3 yaw_pitch_to_vector(f32 yaw, f32 pitch);
void      vector_to_yaw_pitch(glm::vec3 vector, f32* yaw, f32* pitch);
} // namespace sogas
