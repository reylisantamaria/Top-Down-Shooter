#include "engine/ComponentManager.h"
#include "engine/Types.h"
#include <cassert>

void Engine::ComponentManager::EntityDestroyed(Entity entity)
{
  for (const auto &[type, component] : _componentStorage)
  {
    component->EntityDestroyed(entity);
  }
}