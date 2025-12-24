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
   */
  class InputSystem : public Engine::System
  {
    public:
      void Update(const bool *keyboardState);
  };

  /**
   * Movement System
   */
  class MovementSystem : public Engine::System
  {
    public:
      void Update(float dt);
  };


}
