#include "engine/SystemManager.h"

void Engine::SystemManager::EntityDestroyed(Entity entity)
{
  for (const auto &[type, system] : _systems)
  {
    system->_entities.erase(entity);
  }
}

void Engine::SystemManager::EntitySignatureChanged(Entity entity, const Signature &entitySignature)
{
  for (const auto &[type, system] : _systems)
  {
    const auto signatureItr = _signatures.find(type);

    if (signatureItr == _signatures.end())
      continue;

    const auto &systemSignature = signatureItr->second;

    // eg. Entity has:      [1, 1, 1, 0, 0]
    // System requires:     [1, 1, 0, 0, 0]
    // Entity & System:     [1, 1, 0, 0, 0]
    // Comparison:          [1, 1, 0, 0, 0] == [1, 1, 0, 0, 0]
    if ((entitySignature & systemSignature) == systemSignature)
    {
      system->_entities.insert(entity);
    }
    else
    {
      system->_entities.erase(entity);
    }
  }
}
