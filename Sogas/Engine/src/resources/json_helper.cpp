#include "pch.hpp"

#include <resources/json_helper.h>

namespace sogas
{
glm::vec2 load_vec2(const std::string& s)
{
    glm::vec2 v;
    i32       n = sscanf_s(s.c_str(), "%f %f", &v.x, &v.y);

    if (n == 2)
    {
        return v;
    }

    PERROR("Invalid string reading vec2 %s. Only %d values read. Expected 2.", s.c_str(), n);

    return glm::vec2(0.0f);
}

glm::vec2 load_vec2(const json& j, const char* attribute)
{
    ASSERT(j.is_object());

    if (j.count(attribute))
    {
        const std::string& s = j.value(attribute, "");
        return load_vec2(s);
    }

    return glm::vec2(0.0f);
}

glm::vec2 load_vec2(const json& j, const char* attribute, const glm::vec2& default_value)
{
    if (j.count(attribute) == 0)
    {
        return default_value;
    }

    glm::vec2 v = default_value;
    auto      k = j.value(attribute, "0 0");
    sscanf_s(k.c_str(), "%f %f", &v.x, &v.y);
    return v;
}

glm::vec3 load_vec3(const std::string& s)
{
    glm::vec3 v;
    i32       n = sscanf_s(s.c_str(), "%f %f %f", &v.x, &v.y, &v.z);

    if (n == 3)
    {
        return v;
    }

    PERROR("Invalid string reading vec3 %s. Only %d values read. Expected 3.", s.c_str(), n);

    return glm::vec3(0.0f);
}

glm::vec3 load_vec3(const json& j, const char* attribute)
{
    ASSERT(j.is_object());

    if (j.count(attribute))
    {
        const std::string& s = j.value(attribute, "");
        return load_vec3(s);
    }

    return glm::vec3(0.0f);
}

glm::vec3 load_vec3(const json& j, const char* attribute, const glm::vec3& default_value)
{
    if (j.count(attribute) == 0)
    {
        return default_value;
    }

    glm::vec3 v = default_value;
    auto      k = j.value(attribute, "0 0 0");
    sscanf_s(k.c_str(), "%f %f %f", &v.x, &v.y, &v.z);
    return v;
}

glm::vec4 load_vec4(const std::string& s)
{
    glm::vec4 v;
    i32       n = sscanf_s(s.c_str(), "%f %f %f %f", &v.x, &v.y, &v.z, &v.w);

    if (n == 4)
    {
        return v;
    }

    PERROR("Invalid string reading vec4 %s. Only %d values read. Expected 4.", s.c_str(), n);

    return glm::vec4(0.0f);
}

glm::vec4 load_vec4(const json& j, const char* attribute)
{
    ASSERT(j.is_object());

    if (j.count(attribute))
    {
        const std::string& s = j.value(attribute, "");
        return load_vec4(s);
    }

    return glm::vec4(0.0f);
}

glm::vec4 load_vec4(const json& j, const char* attribute, const glm::vec4& default_value)
{
    if (j.count(attribute) == 0)
    {
        return default_value;
    }

    glm::vec4 v = default_value;
    auto      k = j.value(attribute, "0 0 0 0");
    sscanf_s(k.c_str(), "%f %f %f %f", &v.x, &v.y, &v.z, &v.w);
    return v;
}

glm::quat load_quat(const json& j, const char* attribute)
{
    ASSERT(j.is_object());

    if (j.count(attribute))
    {
        const std::string& s = j.value(attribute, "");
        glm::quat          q;
        int                n = sscanf_s(s.c_str(), "%f %f %f %f", &q.x, &q.y, &q.z, &q.w);
        if (n == 4)
        {
            return q;
        }

        PERROR("Invalid json reading quaternion %s. Only %d values read. Expected 4.",
               attribute,
               n);
    }

    return glm::quat();
}

} // namespace sogas
