#pragma once

#include <memory>

#include "Types.h"
#include "ComponentManager.h"
#include "EntityManager.h"
#include "SystemManager.h"

// =======================================================
// Coordinator
// =======================================================

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
    // Get the singleton instance
    static Coordinator &GetInstance();

    // Entities
    Entity CreateEntity();
    void DestroyEntity(Entity entity);

    // Components
    template <typename... ComponentTypes>
    void RegisterComponents();
    template <typename T>
    void AddComponent(Entity entity, T component);
    template <typename T>
    void RemoveComponent(Entity entity);
    template <typename T>
    T &GetComponent(Entity entity);
    template <typename T>
    ComponentType GetComponentType();

    // Systems
    template <typename T, typename... ComponentTypes>
    std::shared_ptr<T> RegisterSystem();
    template <typename T>
    std::shared_ptr<T> GetSystem();

  private:
    std::unique_ptr<ComponentManager> _componentManager; // Manages all component storage and retrieval
    std::unique_ptr<EntityManager> _entityManager;       // Manages entity creation, destruction, and signatures
    std::unique_ptr<SystemManager> _systemManager;       // Manages systems and entity-to-system matching
  };

  // =======================================================
  // Coordinator (template implementations)
  // =======================================================

  
  template <typename... ComponentTypes>
  void Coordinator::RegisterComponents()
  {
    (_componentManager->RegisterComponent<ComponentTypes>(), ...);
  }


  template <typename T>
  void Coordinator::AddComponent(Entity entity, T component)
  {
    _componentManager->AddComponent<T>(entity, component); // Add component to its component array

    auto signature = _entityManager->GetSignature(entity);   // Get current entity signature
    signature.set(_componentManager->GetComponentType<T>()); // Turn on the bit for this component type
    _entityManager->SetSignature(entity, signature);         // Update the entity's signature

    _systemManager->EntitySignatureChanged(entity, signature); // Notify all systems that the entity signature has changed
  }


  template <typename T>
  void Coordinator::RemoveComponent(Entity entity)
  {
    _componentManager->RemoveComponent<T>(entity); // Remove component from its component array

    auto signature = _entityManager->GetSignature(entity);     // Get current entity signature
    signature.reset(_componentManager->GetComponentType<T>()); // Turn off the bit for this component type
    _entityManager->SetSignature(entity, signature);           // Update the entity's signature

    _systemManager->EntitySignatureChanged(entity, signature); // Notify all systems that the entity signature has changed
  }


  template <typename T>
  T &Coordinator::GetComponent(Entity entity)
  {
    return _componentManager->GetComponent<T>(entity);
  }


  template <typename T>
  ComponentType Coordinator::GetComponentType()
  {
    return _componentManager->GetComponentType<T>();
  }


  template <typename T, typename... ComponentTypes>
  std::shared_ptr<T> Coordinator::RegisterSystem()
  {
    static_assert(sizeof...(ComponentTypes) > 0, "System must have at least one component");
    static_assert((std::is_class_v<ComponentTypes> && ...), "All components must be structs/classes");

    // must register the system first
    auto system = _systemManager->RegisterSystem<T>();

    // Create the signature for the system
    Signature signature;
    (signature.set(GetComponentType<ComponentTypes>()), ...);

    // Set the signature for the system
    _systemManager->SetSystemSignature<T>(signature);

    return system;
  }


  template <typename T>
  std::shared_ptr<T> Coordinator::GetSystem()
  {
    return _systemManager->GetSystem<T>();
  }
}
