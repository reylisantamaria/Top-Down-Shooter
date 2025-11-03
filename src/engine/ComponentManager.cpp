#include "engine/ComponentManager.h"
#include "engine/Types.h"
#include <cassert>

// =======================================================
// ComponentManager (non-template function implementations)
// =======================================================

void ECS::ComponentManager::EntityDestroyed(Entity e)
{
  for (auto const &pair : _componentStorage)
  {
    auto const &component = pair.second;
    component->EntityDestroyed(e);
  }
}