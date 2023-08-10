#pragma once

namespace sogas
{
namespace engine
{
class Scene;
namespace handle
{

class HandleManager;

template <typename object_type>
class ObjectManager;

template <typename object_type>
ObjectManager<object_type>* get_object_manager();

class Handle final
{
  public:
    using handle_type       = u32;
    using handle_index      = u32;
    using handle_generation = u32;

    static constexpr u32 num_bits_types      = 7; // 128
    static constexpr u32 num_bits_index      = 14; //
    static constexpr u32 num_bits_generation = 32 - num_bits_types - num_bits_index; //
    static constexpr u32 max_types           = 1 << num_bits_types; // 128

    Handle()
    : type(0),
      index(0),
      generation(0)
    {
    }

    Handle(u32 in_type, u32 in_index, u32 in_generation)
    : type(in_type),
      index(in_index),
      generation(in_generation)
    {
    }

    template <typename object_type>
    Handle(object_type* address)
    {
        auto handle_manager = get_object_manager<std::remove_const<object_type>::type>();
        *this               = handle_manager->get_handle_from_address(address);
    }

    handle_type get_type() const
    {
        return type;
    }

    handle_generation get_generation() const
    {
        return generation;
    }

    handle_index get_external_index(void) const
    {
        return generation;
    }

    bool is_valid() const;

    bool operator==(Handle other) const
    {
        return type == other.type && index == other.index && generation == other.generation;
    }

    bool operator!=(Handle other) const
    {
        return !(*this == other);
    }

    bool operator>(Handle other)
    {
        return type == other.type && index == other.index && generation > other.generation;
    }

    bool operator<(Handle other)
    {
        return type == other.type && index == other.index && generation < other.generation;
    }

    bool operator<=(Handle other)
    {
        return operator==(other) || operator<(other);
    }

    bool operator>=(Handle other)
    {
        return operator==(other) || operator>(other);
    }

    // TODO: Add create function
    // TODO: Add destroy function

    void load(const nlohmann::json& json_data, sogas::engine::Scene& scene);

  private:
    handle_type       type : num_bits_types; // The type of the handle
    handle_index      index : num_bits_index; // Index to find the actual object this handle is owner of.
    handle_generation generation : num_bits_generation; // Make sure there are no old versions of this object.
};

using HandleVector = std::vector<Handle>;

} // namespace handle
} // namespace engine
} // namespace sogas
