#pragma once
#include <bitset>
#include <cstdint>

// Core ECS type definitions
//
// Entity: A unique ID that represents a game object (player, enemy, bullet, etc.)
//         Just a number! The components attached to it define what it is.
//
// ComponentType: A unique ID for each component type (Transform=0, Velocity=1, etc.)
//
// Signature: A bitset that tracks which components an entity has
//            Example: [1,1,0,0,...] means this entity has Transform and Velocity
//            Also used by systems to say "I only care about entities with these components"

namespace ECS
{
  using Entity = std::uint32_t;
  constexpr Entity MAX_ENTITIES = 1000;

  using ComponentType = std::uint32_t;
  constexpr ComponentType MAX_COMPONENTS = 32;

  // Signature: bitset for matching entities to systems
  // If bit 0 is set, the entity has component type 0 (e.g. Transform)
  // If bit 1 is set, the entity has component type 1 (e.g. Velocity)
  using Signature = std::bitset<MAX_COMPONENTS>;
}