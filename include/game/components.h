#pragma once

#include <SDL3/SDL.h>
#include "game/TextureAssets.h"

// =============================================================================
// Game Components
// =============================================================================
// This file contains all component definitions for the ECS (Entity Component System).
// Components are pure data structures with no logic - they only hold state.
//
// Usage:
//   _coordinator.AddComponent<Components::Transform>(entity, Components::Transform{...});
// =============================================================================

namespace Components
{
  /**
   * Transform Component
   * Defines an entity's position, rotation, and rotation center in 2D space.
   */
  struct Transform
  {
    SDL_FPoint position; // World position (x, y)
    float rotation;      // Rotation in degrees (0-360)
    SDL_FPoint scale;    // Scale factor (x, y)
  };

  /**
   * Sprite Component
   * Defines the visual representation of an entity.
   */
  struct Sprite
  {
    TextureID textureId;     // ID of the texture to use
    SDL_FRect srcRect;       // Source rectangle (for sprite sheets)
    SDL_ScaleMode scaleMode; // SDL_SCALEMODE_NEAREST (pixel art) or SDL_SCALEMODE_LINEAR (smooth)
    SDL_FlipMode flipMode;   // Flip mode (horizontal, vertical, or none)
  };

  // TODO: Add more components as needed
  //   Examples: Velocity, Health, Collider, Input, etc.
}