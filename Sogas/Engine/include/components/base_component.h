#pragma once

namespace pinut::resources
{
class CommandBuffer;
}
namespace sogas
{
struct EntityParser;
class BaseComponent
{
  public:
    void update(f32 /*delta_time*/){};
    void on_entity_created(){};
    void render_debug(pinut::resources::CommandBuffer* /*cmd*/){};
    void render_debug_menu(){};
    void load(const nlohmann::json& /*json_file*/, EntityParser& /*scene*/){};
};

// Add this macro in each derived class so that Handle(this)
// finds the game_object_manager<component_type>()
#define DECLARE_SIBILING_ACCESS()             \
    template <typename component_type>        \
    Handle get()                              \
    {                                         \
        Entity* e = Handle(this).get_owner(); \
        if (!e)                               \
        {                                     \
            return Handle();                  \
        }                                     \
        return e->get<component_type>();      \
    }                                         \
    Entity* get_entity()                      \
    {                                         \
        Entity* e = Handle(this).get_owner(); \
        return e;                             \
    }
} // namespace sogas
