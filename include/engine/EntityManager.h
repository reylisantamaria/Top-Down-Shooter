#pragma once

#include <array>
#include <queue>

#include "Types.h"

// =======================================================
// EntityManager: Handles entity lifecycle and signatures
// =======================================================

/* What it does:
 * - Creates entities by assigning unique IDs from a reusable pool
 * - Destroys entities and recycles their IDs for future use
 * - Tracks which components each entity has via signatures
 * - Provides signature lookup for systems to query entity composition
 *
 * What it does NOT do:
 * - Store actual component data (ComponentManager does this)
 * - Decide which systems an entity belongs to (SystemManager does this)
 * - Implement game logic (Systems do this)
 */

namespace Engine
{
  class EntityManager
  {
  public:
    EntityManager();
    Entity CreateEntity();
    void DestroyEntity(Entity entity);
    void SetSignature(Entity entity, Signature signature);
    const Signature &GetSignature(Entity entity) const;

  private:
    std::queue<Entity> _entityIDs{}; // Available entity IDs
                                     // When we destroy an entity, its ID goes back in the queue for reuse

    std::array<Signature, MAX_ENTITIES> _signatures{}; // Signatures for each entity
                                                       // Index by entity ID to get its signature

    size_t _livingEntityCount{}; // Total living entities - keep track for debugging and validation
  };
}
