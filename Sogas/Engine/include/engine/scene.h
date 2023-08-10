#pragma once

#include <handle/handle.h>

namespace sogas
{
namespace engine
{
class Scene
{
  public:
    Scene() = default;

    std::string filename; // The scene file being loaded.

    handle::HandleVector entities;

    // TODO: Scene should have a root transform
    // transform root_transform;

  private:
    void load_scene(const std::string& filename);
};
} // namespace engine
} // namespace sogas