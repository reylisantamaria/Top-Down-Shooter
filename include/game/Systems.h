#pragma once

#include <SDL3/SDL.h>
#include "engine/SystemManager.h"
#include "game/Components.h"
#include "engine/Coordinator.h"

// =======================================================
// Game Systems
// =======================================================

namespace Systems
{
  class RenderSystem : public Engine::System
  {
  public:
    void Update(SDL_Renderer *renderer);
  };
}
