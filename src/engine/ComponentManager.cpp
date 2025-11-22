#include "engine/ComponentManager.h"
#include "engine/Types.h"
#include <cassert>

// =======================================================
// ComponentManager (non-template function implementations)
// =======================================================

void Engine::ComponentManager::EntityDestroyed(Entity e)
{
  for (const auto &[type, component] : _componentStorage)
  {
    component->EntityDestroyed(e);
  }
}