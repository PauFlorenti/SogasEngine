
#include "pch.hpp"

#include <handle/handle.h>
#include <handle/handle_manager.h>

namespace sogas
{
namespace engine
{
namespace handle
{
bool Handle::is_valid() const
{
    auto handle_manager = HandleManager::get_by_type(type);
    return handle_manager->is_valid(*this);
}

void Handle::load(const nlohmann::json& /*json_data*/, sogas::engine::Scene& /*scene*/)
{

}
} // namespace handle
} // namespace engine
} // namespace sogas