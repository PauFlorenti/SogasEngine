#pragma once

namespace sogas
{
glm::vec2 load_vec2(const std::string& s);
glm::vec2 load_vec2(const json& j, const char* attribute);
glm::vec2 load_vec2(const json& j, const char* attribute, const glm::vec2& default_value);
glm::vec3 load_vec3(const std::string& s);
glm::vec3 load_vec3(const json& j, const char* attribute);
glm::vec3 load_vec3(const json& j, const char* attribute, const glm::vec2& default_value);
glm::vec4 load_vec4(const std::string& s);
glm::vec4 load_vec4(const json& j, const char* attribute);
glm::vec4 load_vec4(const json& j, const char* attribute, const glm::vec2& default_value);
glm::quat load_quat(const json& j, const char* attribute);
} // namespace sogas
