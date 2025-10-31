#pragma once

#include <array>
#include <unordered_map>
#include <cassert>

#include "Types.h"

namespace ECS
{
  // =======================================================
  // IComponentArray: Base interface for polymorphism
  // =======================================================
  // Why we need this: ComponentArray<Transform> and ComponentArray<Velocity> are different types,
  // but we want to store them in the same container. This interface lets us do that.
  
  class IComponentArray
  {
  public:
    virtual void EntityDestroyed(Entity e) = 0;
  };

  // =============================================================
  // ComponentArray: Packed storage for a single component type
  // =============================================================

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
  // Template implementations
  // =======================================================
  template <typename T>
  void ComponentArray<T>::AddElement(Entity e, T component)
  {
    // check if the entity already has a component
    assert(_entityToIndex.find(e) == _entityToIndex.end() && "Trying to add a component to the same entity more than once");

    // put new entry at end
    size_t newIndex = _size;

    // update the maps
    _entityToIndex[e] = newIndex;
    _indexToEntity[newIndex] = e;

    // store the actual component data
    _componentArray[newIndex] = component;

    // when successful we must manually adjust the size of the array
    ++_size;
  }

  template <typename T>
  void ComponentArray<T>::RemoveElement(Entity e)
  {
    assert(_entityToIndex.find(e) != _entityToIndex.end() && "You can not remove an entity that doesn't exist");

    // Swap-and-pop algorithm: keeps the array packed with no gaps
    // Instead of leaving a hole, we move the last element into the removed spot
    //
    // Example: removing entity 5 from [1, 3, 5, 7, 9]
    // Step 1: Copy last element (9) into removed spot → [1, 3, 9, 7, 9]
    // Step 2: Shrink size to ignore the duplicate → [1, 3, 9, 7]
    // Result: No gaps, everything still packed!

    size_t removeIndex = _entityToIndex[e];
    size_t lastIndex = _size - 1;
    _componentArray[removeIndex] = _componentArray[lastIndex];

    // update the maps so the moved entity points to its new location
    Entity lastEntity = _indexToEntity[lastIndex];
    _entityToIndex[lastEntity] = removeIndex;
    _indexToEntity[removeIndex] = lastEntity;

    // clean up the removed entity's map entries
    _entityToIndex.erase(e);
    _indexToEntity.erase(lastIndex);

    // one fewer active component
    --_size;
  }

  template <typename T>
  T &ComponentArray<T>::GetData(Entity e)
  {
    // check if it exists
    assert(_entityToIndex.find(e) != _entityToIndex.end() && "You can't request data from an entity that doesn't exist");

    return _componentArray[_entityToIndex[e]];
  }

  template <typename T>
  void ComponentArray<T>::EntityDestroyed(Entity e)
  {
    if (_entityToIndex.find(e) != _entityToIndex.end())
    {
      RemoveElement(e);
    }
  }
}
