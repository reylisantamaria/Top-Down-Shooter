#pragma once

#include <memory>
#include <unordered_map>
#include <typeindex>
#include <cassert>

#include "ComponentArray.h"
#include "Types.h"

// =======================================================
// ComponentManager
// =======================================================

/* What it does:
 * - Registers component types (Transform, Velocity, Health, etc.)
 * - Adds/removes components to/from entities
 * - Retrieves components from entities
 * - Cleans up components when entities are destroyed
 *
 * How it works:
 * - Each component type gets its own packed array for cache performance
 * - Uses templates so you can store any component type
 * - Assigns each type a unique ID for signature matching
 */

namespace ECS
{
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
    std::unordered_map<std::type_index, ComponentType> _componentTypes{}; // Map: component type name → unique ID (0, 1, 2, ...)
                                                                          // Why: We need a number to represent each component type for signatures (bitsets)
                                                                          // Example: Transform=0, Velocity=1, Health=2

    std::unordered_map<std::type_index, std::shared_ptr<IComponentArray>> _componentStorage{}; // Map: component type → its storage array
                                                                                               // Why: Each component type needs its own packed array
                                                                                               // The trick: ComponentArray<Transform> and ComponentArray<Velocity> are different types,
                                                                                               // but they both inherit from IComponentArray, so we can store them together

    ComponentType _nextComponentType{}; // Counter: The next component type ID to assign
                                        // Starts at 0, increments each time we register a new component type

    template <typename T>
    std::shared_ptr<ComponentArray<T>> GetComponentArray();
  };

  // =======================================================
  // ComponentManager (template implementations)
  // =======================================================

  template <typename T>
  void ComponentManager::RegisterComponent()
  {
    std::type_index typeIndex = typeid(T);
    assert(_componentTypes.find(typeIndex) == _componentTypes.end() && "Registering component type more than once.");

    _componentTypes.insert({typeIndex, _nextComponentType});                      // Give this component type a unique ID number
    _componentStorage.insert({typeIndex, std::make_shared<ComponentArray<T>>()}); // Create a new array to store all components of this type

    ++_nextComponentType; // Increment so the next component type gets a different ID
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

  template <typename T>
  std::shared_ptr<ComponentArray<T>> ComponentManager::GetComponentArray()
  {
    std::type_index typeIndex = typeid(T);
    assert(_componentStorage.find(typeIndex) != _componentStorage.end() && "Component not registered before use.");

    return std::static_pointer_cast<ComponentArray<T>>(_componentStorage[typeIndex]); // Cast the generic IComponentArray back to the specific ComponentArray<T> type
                                                                                      // This is safe because we know we stored a ComponentArray<T> when we registered it
  }
}