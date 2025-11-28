#pragma once

#include <array>
#include <unordered_map>
#include <cassert>

#include "Types.h"

namespace Engine
{
  /*
   * Why we need this: ComponentArray<Transform> and ComponentArray<Velocity> are different types,
   * but we want to store them in the same container. This interface lets us do that.
   */
  class IComponentArray
  {
  public:
    virtual ~IComponentArray() = default;
    virtual void EntityDestroyed(Entity entity) = 0;
  };

  /* ComponentArray: Stores all components of a single type (e.g. all Transforms)
   * Why packed arrays? Cache performance. Keeping data tightly packed means faster iteration.
   */
  template <typename T>
  class ComponentArray : public IComponentArray
  {
  public:
    void AddElement(Entity entity, const T &component);  // add a component to an entity
    void RemoveElement(Entity entity);                   // remove a component from an entity (uses swap-and-pop)
    T &GetData(Entity entity);                           // get a component from an entity
    void EntityDestroyed(Entity entity) override;        // remove a component from an entity when it is destroyed

  private:
    std::array<T, MAX_COMPONENTS> _componentArray{};     // The actual component data, stored contiguously for cache performance

    std::unordered_map<Entity, size_t> _entityToIndex{}; // Map: entity ID → array index
                                                         // Why: Quickly find where an entity's component is stored

    std::unordered_map<size_t, Entity> _indexToEntity{}; // Map: array index → entity ID
                                                         // Why: When we remove a component, we need to know which entity we moved

    size_t _size{};                                      // Total count of active components
                                                         // Why track this? The array is fixed size, but only the first _size elements are valid
  };

  // =======================================================

  template <typename T>
  void ComponentArray<T>::AddElement(Entity entity, const T &component)
  {
    assert(_entityToIndex.find(entity) == _entityToIndex.end() && "Trying to add a component to the same entity more than once");

    size_t newIndex = _size;               // Put new entry at end

    _entityToIndex[entity] = newIndex;     // Update entity → index mapping
    _indexToEntity[newIndex] = entity;     // Update index → entity mapping
    _componentArray[newIndex] = component; // Store the actual component data

    ++_size;                               // Increment size to account for new component
  }

  template <typename T>
  void ComponentArray<T>::RemoveElement(Entity entity)
  {
    assert(_entityToIndex.find(entity) != _entityToIndex.end() && "You can not remove an entity that doesn't exist");

    size_t removeIndex = _entityToIndex[entity];               // Find where the removed component is
    size_t lastIndex = _size - 1;                              // Find the last component in the array
    _componentArray[removeIndex] = _componentArray[lastIndex]; // Move last component into the gap

    Entity lastEntity = _indexToEntity[lastIndex];             // Get the entity that was at the last position
    _entityToIndex[lastEntity] = removeIndex;                  // Update its mapping to point to the new location
    _indexToEntity[removeIndex] = lastEntity;                  // Update reverse mapping

    _entityToIndex.erase(entity);                              // Clean up the removed entity's entry
    _indexToEntity.erase(lastIndex);                           // Clean up the old last index entry

    --_size;                                                   // One fewer active component
  }

  template <typename T>
  T &ComponentArray<T>::GetData(Entity entity)
  {
    assert(_entityToIndex.find(entity) != _entityToIndex.end() && "You can't request data from an entity that doesn't exist");

    return _componentArray[_entityToIndex[entity]]; // Look up the index and return the component data
  }

  template <typename T>
  void ComponentArray<T>::EntityDestroyed(Entity entity)
  {
    if (_entityToIndex.find(entity) != _entityToIndex.end())
    {
      RemoveElement(entity);
    }
  }
}
