#include "pch.hpp"

#include <engine/engine.h>
#include <engine/primitives.h>
#include <modules/module_renderer.h>
#include <resources/commandbuffer.h>
#include <resources/mesh.h>

namespace sogas
{
Mesh* line              = nullptr;
Mesh* axis              = nullptr;
Mesh* unit_wired_cube   = nullptr;
Mesh* unit_wired_circle = nullptr;

static void create_line(Mesh& mesh)
{
    mesh.vertices = {{glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), glm::vec2(0.0f)},
                     {{0.0f, 0.0f, 1.0f}, glm::vec3(0.0f), glm::vec3(1.0f), glm::vec2(0.0f)}};

    mesh.upload();
}

static void create_axis(Mesh& mesh)
{
    mesh.vertices = {
      {glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f)},
      {glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f)},
      {glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f)},
      {glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f)},
      {glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f)},
      {glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f)}};

    mesh.upload();
}

static void create_unit_wired_circle(Mesh& mesh)
{
    const i32 samples = 32;
    mesh.vertices.reserve(samples);

    for (i16 i = 0; i < samples; i++)
    {
        f32    angle = 2.0f * glm::pi<f32>() * static_cast<f32>(i) / static_cast<f32>(samples);
        Vertex v;
        v.position = yaw_to_vector(angle);
        v.color    = glm::vec3(1.0f);

        mesh.vertices.push_back(v);
        mesh.indices.push_back(i);
    }

    mesh.upload();
}

static void create_unit_wired_cube(Mesh& mesh)
{
    // TODO primitives do not need color and uvs.
    // clang-format off
    mesh.vertices = {
      {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f), glm::vec3(1.0f), glm::vec2(0.0)},
      {glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(0.0f), glm::vec3(1.0f), glm::vec2(0.0)},
      {glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f), glm::vec3(1.0f), glm::vec2(0.0)},
      {glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3(0.0f), glm::vec3(1.0f), glm::vec2(0.0)},
      {glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f), glm::vec3(1.0f), glm::vec2(0.0)},
      {glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(0.0f), glm::vec3(1.0f), glm::vec2(0.0)},
      {glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.0f), glm::vec3(1.0f), glm::vec2(0.0)},
      {glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3(0.0f), glm::vec3(1.0f), glm::vec2(0.0)}};

    mesh.indices = {
        0, 1, 2, 3, 4, 5, 6, 7,
        0, 2, 1, 3, 4, 6, 5, 7,
        0, 4, 1, 5, 2, 6, 3, 7
    };
    // clang-format on

    mesh.upload();
}

bool create_primitives()
{
    {
        Mesh* mesh = new Mesh();
        mesh->name = "line";
        create_line(*mesh);
        line = mesh;
    }
    {
        Mesh* mesh = new Mesh();
        mesh->name = "axis";
        create_axis(*mesh);
        axis = mesh;
    }
    {
        Mesh* mesh = new Mesh();
        mesh->name = "unit_wired_circle";
        create_unit_wired_circle(*mesh);
        unit_wired_circle = mesh;
    }
    {
        Mesh* mesh = new Mesh();
        mesh->name = "unit_wired_cube";
        create_unit_wired_cube(*mesh);
        unit_wired_cube = mesh;
    }

    return true;
}

void draw_line(pinut::resources::CommandBuffer* cmd,
               const glm::vec3                  origin,
               const glm::vec3                  dest,
               const glm::vec4 /*color*/)
{
    auto            dst  = dest;
    const glm::vec3 dir  = dest - origin;
    f32             dist = glm::length(dir);

    if (dist < 0.001f)
    {
        return;
    }

    // Make sure it does not align with up(0, 1, 0).
    if (glm::abs(dest.x) < 0.001f || glm::abs(dest.z) < 0.001f)
    {
        dst.x += 0.001f;
    }

    auto model = glm::scale(glm::mat4(1.0f), glm::vec3(-dist)) *
                 glm::inverse(glm::lookAt(origin, dst, glm::vec3(0.0f, 1.0f, 0.0f)));

    cmd->set_push_constant(pinut::resources::ShaderStageType::VERTEX, sizeof(glm::mat4), 0, &model);
    line->draw(cmd);
}

void draw_axis(pinut::resources::CommandBuffer* cmd, glm::mat4 transform)
{
    cmd->set_push_constant(pinut::resources::ShaderStageType::VERTEX,
                           sizeof(glm::mat4),
                           0,
                           &transform);
    axis->draw(cmd);
}

void draw_wired_circle_xz(pinut::resources::CommandBuffer* cmd,
                          const glm::mat4&                 transform,
                          const f32                        radius,
                          const glm::vec4 /*color*/)
{
    auto model = glm::scale(transform, glm::vec3(radius));
    cmd->set_push_constant(pinut::resources::ShaderStageType::VERTEX, sizeof(glm::mat4), 0, &model);
    unit_wired_circle->draw_indexed(cmd);
}

void draw_wired_sphere(pinut::resources::CommandBuffer* cmd,
                       const glm::mat4&                 transform,
                       const f32                        radius,
                       const glm::vec4 /*color*/)
{
    auto model = transform * glm::scale(glm::mat4(1.0f), glm::vec3(radius));

    cmd->set_push_constant(pinut::resources::ShaderStageType::VERTEX, sizeof(glm::mat4), 0, &model);
    unit_wired_circle->draw_indexed(cmd);
    auto model_x = glm::rotate(model, glm::pi<f32>() * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
    cmd->set_push_constant(pinut::resources::ShaderStageType::VERTEX,
                           sizeof(glm::mat4),
                           0,
                           &model_x);
    unit_wired_circle->draw_indexed(cmd);
    auto model_z = glm::rotate(model, glm::pi<f32>() * 0.5f, glm::vec3(0.0f, 0.0f, 1.0f));
    cmd->set_push_constant(pinut::resources::ShaderStageType::VERTEX,
                           sizeof(glm::mat4),
                           0,
                           &model_z);
    unit_wired_circle->draw_indexed(cmd);
}

void draw_wired_box(pinut::resources::CommandBuffer* cmd,
                    glm::mat4                        transform,
                    const glm::vec4 /*color*/)
{
    cmd->set_push_constant(pinut::resources::ShaderStageType::VERTEX,
                           sizeof(glm::mat4),
                           0,
                           &transform);
    unit_wired_cube->draw_indexed(cmd);
}

void draw_primitive(pinut::resources::CommandBuffer* cmd,
                    const Mesh*                      mesh,
                    glm::mat4                        model,
                    glm::vec4 /*color*/)
{
    cmd->set_push_constant(pinut::resources::ShaderStageType::VERTEX, sizeof(glm::mat4), 0, &model);
    mesh->draw_indexed(cmd);
}

} // namespace sogas
