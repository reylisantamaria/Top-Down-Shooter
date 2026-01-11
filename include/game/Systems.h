#pragma once

#include <SDL3/SDL.h>
#include "engine/SystemManager.h"
#include "game/Components.h"
#include "engine/Coordinator.h"
#include "engine/TextureManager.h"
#include "game/EntityCreator.h"
#include <cmath>

namespace Systems
{
  // RenderSystem draws entities that have both Transform and Sprite.
  class RenderSystem : public Engine::System
  {
  public:
    void Init(SDL_Renderer *renderer, Engine::TextureManager *textureManager);
    void Update();

  private:
    SDL_Renderer *_renderer;
    Engine::TextureManager *_textureManager;
  };

  // PlayerInputSystem handles WASD input and flags intent components.
  class PlayerInputSystem : public Engine::System
  {
  public:
    void Update();
  };

  // MovementSystem updates transforms based on current velocity.
  class MovementSystem : public Engine::System
  {
  public:
    void Update(float dt);
  };

  // VelocitySystem converts move intents into actual velocity vectors.
  class VelocitySystem : public Engine::System
  {
  public:
    void Update();
  };

  /**
   * Aim System
   * Rotates entities to face the mouse cursor.
   */
  class AimSystem : public Engine::System
  {
  public:
    void Update();
  };

  // WeaponSystem fires weapons when their owners want to shoot.
  class WeaponSystem : public Engine::System
  {
  public:
    void Update();
  };

  // CooldownSystem ticks down cooldown timers.
  class CooldownSystem : public Engine::System
  {
  public:
    void Update(float dt);
  };

  // LifetimeSystem removes entities when their lifetime hits zero.
  class LifetimeSystem : public Engine::System
  {
  public:
    void Update(float dt);
  };
}
