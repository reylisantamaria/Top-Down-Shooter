#pragma once

#include <SDL3/SDL.h>
#include "game/TextureAssets.h"
#include "engine/Vec2.h"

namespace Components
{
  /**
   * Transform Component
   * Defines an entity's position, rotation, and rotation center in 2D space.
   */
  struct Transform
  {
    Engine::Vec2 position;          // World position (x, y)
    float rotation{0.0f};           // Rotation in degrees (0-360)
    Engine::Vec2 scale{1.0f, 1.0f}; // Scale factor (x, y)
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

  /**
   * Player tag (keeps track of the player)
   */
  struct PlayerTag {};

  /**
   * Velocity Component
   */
  struct Velocity
  {
    Engine::Vec2 vel;
    float speed;
  };

  /**
   * Direction Component
   */
  struct Direction
  {
    Engine::Vec2 dir;
  };
}