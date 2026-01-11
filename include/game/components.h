#pragma once

#include <SDL3/SDL.h>
#include "game/TextureAssets.h"
#include "engine/Vec2.h"

namespace Components
{
  // Transform keeps the position, rotation, and scale in 2D.
  struct Transform
  {
    Engine::Vec2 position{0.0f, 0.0f};
    float rotation{0.0f};
    Engine::Vec2 scale{1.0f, 1.0f};
  };

  // Velocity stores the current movement vector.
  struct Velocity
  {
    Engine::Vec2 vector{0.0f, 0.0f};
  };

  // Sprite describes which texture to draw and how to orient it.
  struct Sprite
  {
    TextureID textureId;
    SDL_FRect srcRect;
    SDL_ScaleMode scaleMode;
    SDL_FlipMode flipMode;
    SDL_FPoint pivotPoint;
  };

  // Speed controls how fast the entity moves.
  struct Speed
  {
    float value;
  };

  // Damage tracks how much hurt the entity can deal.
  struct Damage
  {
    float value;
  };

  // Lifetime counts down until the entity expires.
  struct Lifetime
  {
    float remaining;
  };

  // Cooldown prevents actions from happening too soon again.
  struct Cooldown
  {
    float remaining;
  };

  // MoveIntent holds the desired movement direction from input.
  struct MoveIntent
  {
    Engine::Vec2 direction{0.0f, 0.0f};
  };

  // AimIntent remembers the cursor target and aim direction.
  struct AimIntent
  {
    Engine::Vec2 target{0.0f, 0.0f};
    Engine::Vec2 direction{0.0f, 0.0f};
  };

  // FireIntent flags whether the entity wants to shoot.
  struct FireIntent
  {
    bool active{false};
  };

  // WeaponStats groups firing and visual values in one place.
  struct WeaponStats
  {
    // Firing mechanics
    float fireRate;           // Time between shots (seconds)
    float projectileSpeed;    // Projectile velocity (pixels/second)
    float projectileDamage;   // Damage per projectile
    float projectileLifetime; // How long projectiles live (seconds)
    float projectileOffset;   // Offset from owner center (pixels)

    // Projectile visuals
    TextureID projectileTexture;
    SDL_FRect projectileSrcRect;
    SDL_FPoint projectilePivotPoint;
  };

  // OwnedBy keeps a reference to the entity that owns this one.
  struct OwnedBy
  {
    Engine::Entity owner;
  };

  // Tags
  struct Player {}; // Player-controlled entity
  struct Weapon {}; // Weapon entity
}