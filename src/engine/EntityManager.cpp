#include "engine/EntityManager.h"

#include <cassert>

using namespace ECS;

// =======================================================
EntityManager::EntityManager()
{
  // initialize the queue
  for (Entity e = 0; e < MAX_ENTITIES; ++e)
  {
    _EntityIDs.push(e);
  }
}

// =======================================================
Entity EntityManager::CreateEntity()
{
  assert(_LivingEntityCount < MAX_ENTITIES && "Too many entities alive.");

  // pick next unused ID in queue
  Entity id = _EntityIDs.front();

  // remove from queue
  _EntityIDs.pop();
  // mark as used
  ++_LivingEntityCount;

  return id;
}

// =======================================================
void EntityManager::DestroyEntity(Entity e)
{
  assert(e < MAX_ENTITIES && "Entity is out of range.");

  assert(_Signatures[e].none() && "You're trying to destroy an entity with active components.");

  // Since the entity carries it's own valid signature when living
  // must reset the entities signature

  // since entity is only a int, it's used as the index for it's signature
  _Signatures[e].reset();

  // reuse the id before entity is destroyed
  // push the now unused id to the back of the queue
  _EntityIDs.push(e);
  --_LivingEntityCount;
}

// =======================================================
void EntityManager::SetSignature(Entity e, Signature s)
{
  // signatures array is the same size as the max entites.
  assert(e < MAX_ENTITIES && "Entity is out of range.");

  _Signatures[e] = s;
  // current entity's signature is now in the array.
}

// =======================================================
const Signature &EntityManager::GetSignature(Entity e) const
{
  assert(e < MAX_ENTITIES && "Entity is out of range.");

  return _Signatures[e];
}