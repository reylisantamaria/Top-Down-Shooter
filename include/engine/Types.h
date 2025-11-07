#pragma once
#include <bitset>
#include <cstdint>

// =======================================================
// Core ECS Type Definitions
// =======================================================

/*
 * Entity:
 *   A unique ID representing a game object (player, enemy, bullet, etc.)
 *   Just a number; The components attached to it define what it is.
 *
 * ComponentType:
 *   A unique ID for each component type (Transform=0, Velocity=1, etc.)
 *
 * Signature:
 *   A bitset tracking which components an entity has.
 *   Example: [1,1,0,0,...] means this entity has Transform and Velocity.
 *   Also used by systems to specify required components.
 */

namespace Engine
{
  using Entity = std::uint32_t;
  constexpr Entity MAX_ENTITIES = 1000;

  using ComponentType = std::uint32_t;
  constexpr ComponentType MAX_COMPONENTS = 32;

  using Signature = std::bitset<MAX_COMPONENTS>;
}