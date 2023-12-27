#pragma once

namespace sogas
{
struct EntityParser
{
    std::string   filename;
    EntityParser* parent = nullptr;
    HandleVector  entities; // Root entities
    HandleVector  all_entities; // All entities, including childs from root entities.
    glm::mat4     transform;
};

bool parse_scene(const std::string& filename, EntityParser& context);
} // namespace sogas
