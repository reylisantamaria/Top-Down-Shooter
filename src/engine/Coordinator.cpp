#include "engine/Coordinator.h"

// =======================================================
// Coordinator (non-template function implementations)
// =======================================================

void Engine::Coordinator::Init()
{
  _componentManager = std::make_unique<ComponentManager>();
  _entityManager = std::make_unique<EntityManager>();
  _systemManager = std::make_unique<SystemManager>();
}

Engine::Entity Engine::Coordinator::CreateEntity()
{
  return _entityManager->CreateEntity();
}

void Engine::Coordinator::DestroyEntity(Entity entity)
{
  _systemManager->EntityDestroyed(entity);                // Remove entity from all systems first
  _componentManager->EntityDestroyed(entity);             // Remove all components from entity
  _entityManager->SetSignature(entity, Engine::Signature()); // Reset the entity's signature
  _entityManager->DestroyEntity(entity);                  // Finally destroy the entity (this checks signature is empty)
}