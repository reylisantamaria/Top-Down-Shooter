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

  /*
   * Why we need this: ComponentArray<Transform> and ComponentArray<Velocity> are different types,
   * but we want to store them in the same container. This interface lets us do that.
   */
  class IComponentArray
  {
  public:
    virtual ~IComponentArray() = default;
    virtual void EntityDestroyed(Entity e) = 0;
  };
  
  // =============================================================
  // ComponentArray: Packed storage for a single component type
  // =============================================================

  /* ComponentArray: Stores all components of a single type (e.g. all Transforms)
   * Why packed arrays? Cache performance. Keeping data tightly packed means faster iteration.
   */
  template <typename T>
  class ComponentArray : public IComponentArray
  {
  public:
    void AddElement(Entity e, T component);  // add a component to an entity
    void RemoveElement(Entity e);            // remove a component from an entity (uses swap-and-pop)
    T &GetData(Entity e);                    // get a component from an entity
    void EntityDestroyed(Entity e) override; // remove a component from an entity when it is destroyed

  private:
    std::array<T, MAX_COMPONENTS> _componentArray{}; // The actual component data, stored contiguously for cache performance

    std::unordered_map<Entity, size_t> _entityToIndex{}; // Map: entity ID → array index
                                                         // Why: Quickly find where an entity's component is stored

    std::unordered_map<size_t, Entity> _indexToEntity{}; // Map: array index → entity ID
                                                         // Why: When we remove a component, we need to know which entity we moved

    size_t _size{}; // Total count of active components
                    // Why track this? The array is fixed size, but only the first _size elements are valid
  };

  // =======================================================
  // ComponentArray (template implementations)
  // =======================================================

  template <typename T>
  void ComponentArray<T>::AddElement(Entity e, T component)
  {
    assert(_entityToIndex.find(e) == _entityToIndex.end() && "Trying to add a component to the same entity more than once");

    size_t newIndex = _size; // Put new entry at end

    _entityToIndex[e] = newIndex;          // Update entity → index mapping
    _indexToEntity[newIndex] = e;          // Update index → entity mapping
    _componentArray[newIndex] = component; // Store the actual component data

    ++_size; // Increment size to account for new component
  }

  template <typename T>
  void ComponentArray<T>::RemoveElement(Entity e)
  {
    assert(_entityToIndex.find(e) != _entityToIndex.end() && "You can not remove an entity that doesn't exist");

    size_t removeIndex = _entityToIndex[e];                    // Find where the removed component is
    size_t lastIndex = _size - 1;                              // Find the last component in the array
    _componentArray[removeIndex] = _componentArray[lastIndex]; // Move last component into the gap

    Entity lastEntity = _indexToEntity[lastIndex]; // Get the entity that was at the last position
    _entityToIndex[lastEntity] = removeIndex;      // Update its mapping to point to the new location
    _indexToEntity[removeIndex] = lastEntity;      // Update reverse mapping

    _entityToIndex.erase(e);         // Clean up the removed entity's entry
    _indexToEntity.erase(lastIndex); // Clean up the old last index entry

    --_size; // One fewer active component
  }

  template <typename T>
  T &ComponentArray<T>::GetData(Entity e)
  {
    assert(_entityToIndex.find(e) != _entityToIndex.end() && "You can't request data from an entity that doesn't exist");

    return _componentArray[_entityToIndex[e]]; // Look up the index and return the component data
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
