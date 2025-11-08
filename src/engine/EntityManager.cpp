#include "engine/EntityManager.h"

#include <cassert>
  
// =======================================================
// EntityManager (non-template function implementations)
// =======================================================

Engine::EntityManager::EntityManager()
{
  // initialize the queue
  for (Entity entity = 0; entity < MAX_ENTITIES; ++entity)
  {
    _entityIDs.push(entity);
  }
}

// =======================================================
Engine::Entity Engine::EntityManager::CreateEntity()
{
  assert(_livingEntityCount < MAX_ENTITIES && "Too many entities alive.");

  // pick next unused ID in queue
  Entity id = _entityIDs.front();

  // remove from queue
  _entityIDs.pop();
  // mark as used
  ++_livingEntityCount;

  return id;
}

// =======================================================
void Engine::EntityManager::DestroyEntity(Entity entity)
{
  assert(entity < MAX_ENTITIES && "Entity is out of range.");

  assert(_signatures[entity].none() && "You're trying to destroy an entity with active components.");

  // Since the entity carries it's own valid signature when living
  // must reset the entities signature

  // since entity is only a int, it's used as the index for it's signature
  _signatures[entity].reset();

  // reuse the id before entity is destroyed
  // push the now unused id to the back of the queue
  _entityIDs.push(entity);
  --_livingEntityCount;
}

// =======================================================
void Engine::EntityManager::SetSignature(Entity entity, Signature signature)
{
  // signatures array is the same size as the max entites.
  assert(entity < MAX_ENTITIES && "Entity is out of range.");

  _signatures[entity] = signature;
  // current entity's signature is now in the array.
}

// =======================================================
const Engine::Signature &Engine::EntityManager::GetSignature(Entity entity) const
{
  assert(entity < MAX_ENTITIES && "Entity is out of range.");

  return _signatures[entity];
}

// =======================================================
size_t Engine::EntityManager::GetLivingEntityCount() const
{
  return _livingEntityCount;
}