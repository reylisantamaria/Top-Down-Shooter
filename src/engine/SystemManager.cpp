#include "engine/SystemManager.h"

// =======================================================
// SystemManager (non-template function implementations)
// =======================================================

void Engine::SystemManager::EntityDestroyed(Entity entity)
{
  for (const auto &pair : _systems)
  {
    const auto &system = pair.second;
    system->_entitiesSet.erase(entity);
  }
}

void Engine::SystemManager::EntitySignatureChanged(Entity entity, Signature entitySignature)
{
  for (const auto &pair : _systems)
  {
    const auto &type = pair.first;
    const auto &signatureItr = _signatures.find(type);

    if (signatureItr == _signatures.end())
      continue;

    const auto &systemSignature = signatureItr->second;
    const auto &system = pair.second;

    // eg. Entity has:      [1, 1, 1, 0, 0]
    // System requires:     [1, 1, 0, 0, 0]
    // Entity & System:     [1, 1, 0, 0, 0]
    // Comparison:          [1, 1, 0, 0, 0] == [1, 1, 0, 0, 0]
    if ((entitySignature & systemSignature) == systemSignature)
    {
      system->_entitiesSet.insert(entity);
    }
    else
    {
      system->_entitiesSet.erase(entity);
    }
  }
}
