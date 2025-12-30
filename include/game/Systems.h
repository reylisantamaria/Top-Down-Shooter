#pragma once

#include <SDL3/SDL.h>
#include "engine/SystemManager.h"
#include "game/Components.h"
#include "engine/Coordinator.h"
#include "engine/TextureManager.h"

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

  /**
   * Input System
   * Handles player keyboard input (WASD) and sets velocity accordingly.
   */
  class InputSystem : public Engine::System
  {
  public:
    void Update();
  };

  /**
   * Movement System
   * Moves all entities based on their velocity.
   */
  class MovementSystem : public Engine::System
  {
  public:
    void Update(float dt);
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

}
