#pragma once

#include <SDL3/SDL.h>
#include "engine/SystemManager.h"
#include "game/Components.h"
#include "engine/Coordinator.h"
#include "engine/TextureManager.h"

// =============================================================================
// Game Systems
// =============================================================================
// This file contains all system definitions for the ECS (Entity Component System).
// Systems contain the game logic and operate on entities with specific components.
//
// Usage:
//   auto renderSystem = _coordinator.RegisterSystem<Systems::RenderSystem>();
//   renderSystem->Update(renderer);
// =============================================================================

namespace Systems
{
  /**
   * Render System
   * Renders all entities that have both Transform and Sprite components.
   */
  class RenderSystem : public Engine::System
  {
  public:
    void Init(SDL_Renderer *renderer, Engine::TextureManager *textureManager);
    void Update();

  private:
    SDL_Renderer *_renderer;
    Engine::TextureManager *_textureManager;
  };

  // TODO: Add more systems as needed
  //   Examples: MovementSystem, PhysicsSystem, CollisionSystem, InputSystem, etc.
}
