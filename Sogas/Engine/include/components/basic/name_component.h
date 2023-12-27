
#include <components/base_component.h>
#include <handle/handle.h>

namespace sogas
{
class NameComponent : public BaseComponent
{
  public:
    static std::unordered_map<std::string, Handle> all_names;

    const std::string get_name() const;
    void              set_name(const std::string& name);
    void              load(const json& j, EntityParser& context);

  private:
    static constexpr size_t max_length = 128;
    char                    name[max_length];
};
} // namespace sogas
