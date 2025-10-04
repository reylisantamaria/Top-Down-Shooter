#pragma once
#include <bitset>
#include <cstdint>

// std::uint32_t is a 32-bit unsigned integer
// std::bitset is a bitset class that can be used to store a set of bits

// signature is used to store the components that an entity has

// Entity - just a type alias for entity IDs

namespace ECS
{
  using Entity = std::uint32_t;
  constexpr Entity MAX_ENTITIES = 5000;

  // Component - empty base struct for type safety and inheritance
  using Component = std::uint32_t;
  constexpr Component MAX_COMPONENTS = 32;

  // Signature - bitset for component filtering and system matching
  using Signature = std::bitset<MAX_COMPONENTS>;
}