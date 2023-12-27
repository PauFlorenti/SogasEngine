#include "pch.hpp"
#include <entity/entity.h>
#include <entity/entity_parser.h>

namespace sogas
{
bool parse_scene(const std::string& filename, EntityParser& context)
{
    context.filename = filename;

    const auto& json = platform::load_json(filename);

    ASSERT(json.is_array());

    for (i32 i = 0; i < json.size(); ++i)
    {
        const auto& jscene = json[i];
        ASSERT(jscene.is_object());

        if (jscene.count("entity"))
        {
            // TODO Prefab ??.

            const auto& jentity = jscene["entity"];

            Handle entity_handle;

            entity_handle.create<Entity>();
            Entity* entity = entity_handle;
            entity->load(jentity, context);

            context.all_entities.push_back(entity_handle);
            context.entities.push_back(entity_handle);
        }
    }

    for (auto h : context.all_entities)
    {
        h.on_entity_created();
    }

    return true;
}
} // namespace sogas
