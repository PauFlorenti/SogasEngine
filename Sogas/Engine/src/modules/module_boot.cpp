#include <entity/entity_parser.h>
#include <modules/module_boot.h>

namespace sogas
{
namespace modules
{
bool BootModule::start()
{
    json json   = platform::load_json("../../Sogas/Engine/data/boot.json");
    auto scenes = json["scenes"].get<std::vector<std::string>>();

    for (auto scene : scenes)
    {
        load_scene(scene);
    }

    return true;
}

void BootModule::load_scene(const std::string& filename)
{
    PDEBUG("Parsing %s", filename.c_str());

    EntityParser context;
    parse_scene(filename, context);
}
} // namespace modules
} // namespace sogas
