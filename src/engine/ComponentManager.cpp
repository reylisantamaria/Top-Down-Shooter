#include "engine/ComponentManager.h"
#include "engine/Types.h"
#include <cassert>

using namespace ECS;

// ============================================================
//                    COMPONENT ARRAYS
// ============================================================
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

// ============================================================
//                    COMPONENT MANAGER
// ============================================================

template <typename T>
void ComponentManager::RegisterComponent()
{
  std::type_index typeIndex = typeid(T);
  assert(_componentTypes.find(typeIndex) == _componentTypes.end() && "Registering component type more than once.");

  // give this component type a unique ID number
  _componentTypes.insert({typeIndex, _nextComponentType});

  // create a new array to store all components of this type
  _componentStorage.insert({typeIndex, std::make_shared<ComponentArray<T>>()});

  // increment so the next component type gets a different ID
  ++_nextComponentType;
}

template <typename T>
ComponentType ComponentManager::GetComponentType()
{
  std::type_index typeIndex = typeid(T);
  assert(_componentTypes.find(typeIndex) != _componentTypes.end() && "Component not registered before use.");
  return _componentTypes[typeIndex];
}

template <typename T>
void ComponentManager::AddComponent(Entity e, T component)
{
  GetComponentArray<T>()->AddElement(e, component);
}

template <typename T>
void ComponentManager::RemoveComponent(Entity e)
{
  GetComponentArray<T>()->RemoveElement(e);
}

template <typename T>
T &ComponentManager::GetComponent(Entity e)
{
  return GetComponentArray<T>()->GetData(e);
}

void ComponentManager::EntityDestroyed(Entity e)
{
  for (auto const &pair : _componentStorage)
  {
    auto const &component = pair.second;
    component->EntityDestroyed(e);
  }
}

template <typename T>
std::shared_ptr<ComponentArray<T>> ComponentManager::GetComponentArray()
{
  std::type_index typeIndex = typeid(T);
  assert(_componentStorage.find(typeIndex) != _componentStorage.end() && "Component not registered before use.");

  // cast the generic IComponentArray back to the specific ComponentArray<T> type
  // this is safe because we know we stored a ComponentArray<T> when we registered it
  return std::static_pointer_cast<ComponentArray<T>>(_componentStorage[typeIndex]);
}