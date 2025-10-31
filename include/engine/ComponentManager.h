#pragma once

#include <typeindex>

#include "Types.h"
#include "EntityManager.h"

// ComponentManager: Stores all components for all entities in the game
// What it does:
// 1. Add a component to an entity
// 2. Access a component from a specific entity
// 3. Remove a component from an entity
// 4. Clean up components when entities are destroyed
//
// How it works:
// - Each component type (Transform, Velocity, etc.) gets its own packed array
// - Uses templates so you can store any component type
// - Keeps components tightly packed in memory for better performance

namespace ECS
{
  // =======================================================

  // Interface for component arrays (so we can store different types together)
  // Why we need this: ComponentArray<Transform> and ComponentArray<Velocity> are different types,
  // but we want to store them in the same container. This interface lets us do that.
  class IComponentArray
  {
  public:
    virtual void EntityDestroyed(Entity e) = 0;
  };

  // ComponentArray: Stores all components of a single type (e.g. all Transforms)
  // Why packed arrays? Cache performance. Keeping data tightly packed means faster iteration.
  template <typename T>
  class ComponentArray : public IComponentArray
  {
  public:
    void AddElement(Entity e, T component); // add a component to an entity
    void RemoveElement(Entity e);           // remove a component from an entity (uses swap-and-pop)
    T &GetData(Entity e);                   // get a component from an entity
    void EntityDestroyed(Entity e) override;

  private:
    // The actual component data, stored contiguously
    std::array<T, MAX_COMPONENTS> _componentArray{};

    // Two-way mapping between entities and array indices
    // Why both directions?
    // - entityToIndex: quickly find where an entity's component is stored
    // - indexToEntity: when we remove a component, we need to know which entity we moved
    std::unordered_map<Entity, size_t> _entityToIndex{};
    std::unordered_map<size_t, Entity> _indexToEntity{};

    // Total count of active components
    // Why track this? The array is fixed size, but only the first _size elements are valid
    size_t _size{};
  };

  // =======================================================
  class ComponentManager
  {
  public:
    template <typename T>
    void RegisterComponent();

    template <typename T>
    ComponentType GetComponentType();

    template <typename T>
    void AddComponent(Entity e, T component);

    template <typename T>
    void RemoveComponent(Entity e);

    template <typename T>
    T &GetComponent(Entity e);

    void EntityDestroyed(Entity e);

  private:
    // Map: component type name → unique ID (0, 1, 2, ...)
    // Why: We need a number to represent each component type for signatures (bitsets)
    // Example: Transform=0, Velocity=1, Health=2
    std::unordered_map<std::type_index, ComponentType> _componentTypes{};

    // Map: component type → its storage array
    // Why: Each component type needs its own packed array
    // The trick: ComponentArray<Transform> and ComponentArray<Velocity> are different types,
    //            but they both inherit from IComponentArray, so we can store them together
    // Example: Transform → ComponentArray<Transform>, Velocity → ComponentArray<Velocity>
    std::unordered_map<std::type_index, std::shared_ptr<IComponentArray>> _componentStorage{};

    // Counter: The next component type ID to assign
    // Starts at 0, increments each time we register a new component type
    ComponentType _nextComponentType{};

    template <typename T>
    std::shared_ptr<ComponentArray<T>> GetComponentArray();
  };
}