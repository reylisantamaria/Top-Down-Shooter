#pragma once

#include <memory>
#include <unordered_map>
#include <typeindex>
#include <cassert>
#include "ComponentArray.h"
#include "Types.h"


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

namespace Engine
{
  class ComponentManager
  {
  public:
    template <typename T>
    void RegisterComponent();

    template <typename T>
    ComponentType GetComponentType();

    template <typename T>
    void AddComponent(Entity entity, const T &component);

    template <typename T>
    void RemoveComponent(Entity entity);

    template <typename T>
    T &GetComponent(Entity entity) const;

    void EntityDestroyed(Entity entity);

  private:
    std::unordered_map<std::type_index, ComponentType> _componentTypes{};                      // Map: component type name → unique ID (0, 1, 2, ...)
                                                                                               // Why: We need a number to represent each component type for signatures (bitsets)
                                                                                               // Example: Transform=0, Velocity=1, Health=2

    std::unordered_map<std::type_index, std::shared_ptr<IComponentArray>> _componentStorage{}; // Map: component type → its storage array
                                                                                               // Why: Each component type needs its own packed array
                                                                                               // The trick: ComponentArray<Transform> and ComponentArray<Velocity> are different types,
                                                                                               // but they both inherit from IComponentArray, so we can store them together

    ComponentType _nextComponentType{};                                                        // Counter: The next component type ID to assign
                                                                                               // Starts at 0, increments each time we register a new component type

    template <typename T>
    std::shared_ptr<ComponentArray<T>> GetComponentArray() const;
  };

  // =======================================================

  template <typename T>
  void ComponentManager::RegisterComponent()
  {
    std::type_index typeIndex = typeid(T);
    if (_componentTypes.find(typeIndex) != _componentTypes.end())                 // Silently return if already registered (allows auto-registration to be safe)
    {
      return;
    }

    _componentTypes.insert({typeIndex, _nextComponentType});                      // Give this component type a unique ID number
    _componentStorage.insert({typeIndex, std::make_shared<ComponentArray<T>>()}); // Create a new array to store all components of this type

    ++_nextComponentType;                                                         // Increment so the next component type gets a different ID
  }

  template <typename T>
  ComponentType ComponentManager::GetComponentType()
  {
    std::type_index typeIndex = typeid(T);

    // Auto-register component if not already registered (like EnTT)
    if (_componentTypes.find(typeIndex) == _componentTypes.end())
    {
      RegisterComponent<T>();
    }

    return _componentTypes[typeIndex];
  }

  template <typename T>
  void ComponentManager::AddComponent(Entity entity, const T &component)
  {
    GetComponentArray<T>()->AddElement(entity, component);
  }

  template <typename T>
  void ComponentManager::RemoveComponent(Entity entity)
  {
    GetComponentArray<T>()->RemoveElement(entity);
  }

  template <typename T>
  T &ComponentManager::GetComponent(Entity entity) const
  {
    return GetComponentArray<T>()->GetData(entity);
  }

  template <typename T>
  std::shared_ptr<ComponentArray<T>> ComponentManager::GetComponentArray() const
  {
    std::type_index typeIndex = typeid(T);
    assert(_componentStorage.find(typeIndex) != _componentStorage.end() && "Component not registered before use.");

    return std::static_pointer_cast<ComponentArray<T>>(_componentStorage.at(typeIndex)); // Cast the generic IComponentArray back to the specific ComponentArray<T> type
                                                                                      // This is safe because we know we stored a ComponentArray<T> when we registered it
  }
}