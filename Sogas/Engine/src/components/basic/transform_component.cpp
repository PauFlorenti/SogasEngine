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
} // namespace sogas
