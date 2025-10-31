#include "engine/ComponentManager.h"
#include "engine/Types.h"
#include <cassert>

using namespace ECS;

// =======================================================
// Non-template function implementations
// =======================================================

void ComponentManager::EntityDestroyed(Entity e)
{
  for (auto const &pair : _componentStorage)
  {
    auto const &component = pair.second;
    component->EntityDestroyed(e);
  }
}