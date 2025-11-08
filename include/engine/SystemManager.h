#pragma once

#include <memory>
#include <set>
#include <unordered_map>
#include <typeindex>
#include <cassert>

#include "Types.h"

namespace Engine
{
  // =======================================================
  // System: Base class for all game logic systems
  // =======================================================

  /*
   * Implements game logic by looping through entities it cares about
   *
   * What it does:
   * - Holds a set of entities it should process
   * - Subclasses implement Update() or similar methods with actual logic
   *
   * What it does NOT do:
   * - Decide which entities belong to it (SystemManager does this)
   * - Know what signature it requires (SystemManager handles this)
   * - Add/remove entities from its own list (SystemManager manages this)
   */

  class System
  {
  public:
    std::set<Entity> _entitiesSet; // Which entities this system cares about
                                   // Updated automatically by SystemManager when entity signatures change
  };

  // =======================================================
  // SystemManager: Matchmaker between entities and systems
  // =======================================================

  /*
   * Figures out which entities belong in which systems based on component signatures
   * Watches for signature changes and updates system entity lists accordingly
   * Cleans up when entities are destroyed
   */

  class SystemManager
  {
  public:
    template <typename T>
    std::shared_ptr<T> RegisterSystem();

    template <typename T>
    std::shared_ptr<T> GetSystem();

    template <typename T>
    void SetSystemSignature(Signature signature);

    void EntityDestroyed(Entity entity);

    void EntitySignatureChanged(Entity entity, Signature entitySignature);

  private:
    std::unordered_map<std::type_index, std::shared_ptr<System>> _systems{}; // Map: system type → its implementation
                                                                             // Why: We need to store the actual System objects
                                                                             // Example: PhysicsSystem → System implementation

    std::unordered_map<std::type_index, Signature> _signatures{}; // Map: system type → its required component signature
                                                                  // Why: We need to know which components each system requires
                                                                  // Example: PhysicsSystem requires Transform and Velocity components
  };

  // =======================================================
  // Template implementations
  // =======================================================

  template <typename T>
  std::shared_ptr<T> SystemManager::RegisterSystem()
  {
    std::type_index typeIndex = typeid(T);
    assert(_systems.find(typeIndex) == _systems.end() && "System already exists!");

    auto system = std::make_shared<T>();  // Create the system instance
    _systems.insert({typeIndex, system}); // Store it in our map

    return system; // Return the system so the caller can configure it
  }

  template <typename T>
  std::shared_ptr<T> SystemManager::GetSystem()
  {
    std::type_index typeIndex = typeid(T);
    assert(_systems.find(typeIndex) != _systems.end() && "The system doesn't exist");
    return std::static_pointer_cast<T>(_systems[typeIndex]);
  }

  template <typename T>
  void SystemManager::SetSystemSignature(Signature signature)
  {
    std::type_index typeIndex = typeid(T);
    assert(_systems.find(typeIndex) != _systems.end() && "The system doesn't exist");

    _signatures[typeIndex] = signature; // Store the signature so we can match entities to this system
  }
}