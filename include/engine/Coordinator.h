#pragma once

#include <memory>
#include <concepts>
#include "Types.h"
#include "ComponentManager.h"
#include "EntityManager.h"
#include "SystemManager.h"

/* What it does:
 * - Manages the creation, destruction, and component signatures of entities
 * - Manages the registration, addition, and removal of components
 * - Manages the registration, addition, and removal of systems
 * - Manages the matching of entities to systems based on component signatures
 */
namespace Engine
{
  class Coordinator
  {
  public:
    static Coordinator &GetInstance();                          // Get the singleton instance

    Entity CreateEntity();                                      // Create a new entity
    void DestroyEntity(Entity entity);                          // Destroy an entity
    std::size_t GetEntityCount() const;                         // Get the number of entities

    template <typename T>
      requires std::is_class_v<T>
    void AddComponent(Entity entity, const T &component);       // Add a component to an entity
    template <typename T>
      requires std::is_class_v<T>
    void RemoveComponent(Entity entity);                        // Remove a component from an entity
    template <typename T>
      requires std::is_class_v<T>
    T &Get(Entity entity) const;                                // Get a component from an entity
    template <typename T>
      requires std::is_class_v<T>
    T *GetOptional(Entity entity) const;                        // Safe to request a component that may not exist

    template <typename T, typename... Components>
    std::shared_ptr<T> RegisterSystem();                        // Register a system
  private:
    Coordinator();

    template <typename T>
    ComponentType GetComponentType();                     // Get the component type for a given component (helper)

    std::unique_ptr<ComponentManager> _componentManager;        // Manages all component storage and retrieval
    std::unique_ptr<EntityManager> _entityManager;              // Manages entity creation, destruction, and signatures
    std::unique_ptr<SystemManager> _systemManager;              // Manages systems and entity-to-system matching
  };

  // =======================================================

  template <typename T>
  requires std::is_class_v<T>
  void Coordinator::AddComponent(Entity entity, const T &component)
  {
    _componentManager->AddComponent<T>(entity, component);     // Add component to its component array

    auto signature = _entityManager->GetSignature(entity);     // Get current entity signature
    signature.set(_componentManager->GetComponentType<T>());   // Turn on the bit for this component type
    _entityManager->SetSignature(entity, signature);           // Update the entity's signature

    _systemManager->EntitySignatureChanged(entity, signature); // Notify all systems that the entity signature has changed
  }

  template <typename T>
    requires std::is_class_v<T>
  void Coordinator::RemoveComponent(Entity entity)
  {
    _componentManager->RemoveComponent<T>(entity);             // Remove component from its component array

    auto signature = _entityManager->GetSignature(entity);     // Get current entity signature
    signature.reset(_componentManager->GetComponentType<T>()); // Turn off the bit for this component type
    _entityManager->SetSignature(entity, signature);           // Update the entity's signature

    _systemManager->EntitySignatureChanged(entity, signature); // Notify all systems that the entity signature has changed
  }

  template <typename T>
    requires std::is_class_v<T>
  T &Coordinator::Get(Entity entity) const
  {
    return _componentManager->GetComponent<T>(entity);
  }
  
  template <typename T, typename... Components>
  std::shared_ptr<T> Coordinator::RegisterSystem()
  {
    static_assert(sizeof...(Components) > 0, "System must have at least one component");
    static_assert((std::is_class_v<Components> && ...), "All components must be structs/classes");

    // Must register the system first
    auto system = _systemManager->RegisterSystem<T>();

    // Create the signature for the system
    Signature signature;
    (signature.set(GetComponentType<Components>()), ...);

    // Set the signature for the system
    _systemManager->SetSystemSignature<T>(signature);

    return system;
  }

  template <typename Component>
    requires std::is_class_v<Component>
  Component *Coordinator::GetOptional(Entity entity) const
  {
    ComponentType componentType = GetComponentType<Component>();

    if (_entityManager->GetSignature(entity).test(componentType)) {
      return &_componentManager->GetComponent<Component>(entity);
    }
    return nullptr;
  }

  template <typename T>
  ComponentType Coordinator::GetComponentType()
  {
    return _componentManager->GetComponentType<T>();
  }
}
