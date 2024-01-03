#pragma once

namespace pinut::resources
{
class CommandBuffer;
}

namespace sogas
{
class Mesh;

bool create_primitives();

void draw_line(pinut::resources::CommandBuffer* cmd,
               const glm::vec3                  origin,
               const glm::vec3                  dest,
               const glm::vec4                  color = glm::vec4(1.0f));
void draw_axis(pinut::resources::CommandBuffer* cmd, glm::mat4 transform);
void draw_wired_circle_xz(pinut::resources::CommandBuffer* cmd,
                          const glm::mat4&                 transform,
                          const f32                        radius = 1.0f,
                          const glm::vec4                  color  = glm::vec4(1.0f));
void draw_wired_sphere(pinut::resources::CommandBuffer* cmd,
                       const glm::mat4&                 transform,
                       const f32                        radius = 1.0f,
                       const glm::vec4                  color  = glm::vec4(1.0f));

void draw_wired_box(pinut::resources::CommandBuffer* cmd,
                    glm::mat4                        transform,
                    const glm::vec4                  color = glm::vec4(1.0f));

void draw_primitive(pinut::resources::CommandBuffer* cmd,
                    const Mesh*                      mesh,
                    glm::mat4                        model,
                    const glm::vec4                  color = glm::vec4(1.0f));
} // namespace sogas
