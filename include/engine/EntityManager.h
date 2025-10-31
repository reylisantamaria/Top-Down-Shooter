#pragma once

#include <array>
#include <queue>

#include "Types.h"

// 1. we must be able to create and entity
//    -> pick unused ID -> mark it as "in use" -> give it back to the caller 

// 2. we can destroy an entity but can reuse the ID for later

// 3. set signatures to entities

// 4. get signatures from entities

namespace ECS
{
  class EntityManager
  {
  public:
    EntityManager();
    Entity CreateEntity();
    void DestroyEntity(Entity e);
    void SetSignature(Entity e, Signature s);
    const Signature& GetSignature(Entity e) const;

  private:
    // available entity ids
    std::queue<Entity> _EntityIDs{};

    // signatures for each entity
    std::array<Signature, MAX_ENTITIES> _Signatures{};

    // Total living entites - keep track
    size_t _LivingEntityCount{};
  };
}
