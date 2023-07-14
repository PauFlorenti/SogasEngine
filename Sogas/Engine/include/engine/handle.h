#pragma once

namespace sogas::engine
{
class Handle final
{
    using handle_type       = u32;
    using handle_index      = u32;
    using handle_generation = u32;

  public:
    static constexpr u32 num_bits_types      = 7; // 128
    static constexpr u32 num_bits_index      = 14; // 128
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

    handle_type get_type() const
    {
        return type;
    }

    handle_generation get_generation() const
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

  private:
    handle_type       type : num_bits_type; // The type of the handle
    handle_index      index : num_bits_index; // Index to find the actual object this handle is owner of.
    handle_generation generation : num_bits_generation; // Make sure there are no old versions of this object.
};
} // namespace sogas::engine
