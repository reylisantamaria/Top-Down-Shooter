# ECS Engine

A custom Entity Component System (ECS) implementation for a 2D top-down shooter using SDL3 and C++.

This is my take on an ECS. It uses packed arrays for cache-friendly performance and automatic signature-based entity-to-system matching. This is overkill for a casual game like this, but I wanted to challenge myself to understand how ECS works.

## Coordinator API Reference

All ECS operations go through the `Coordinator` singleton. Here's every function available:

| Category                 | Function                                                    | Description                                                                                                                                             | Returns                       |
| ------------------------ | ----------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------- | ----------------------------- |
| **Singleton**            | `GetInstance()`                                             | Get the Coordinator singleton instance                                                                                                                  | `Coordinator&`                |
| **Entity Management**    | `CreateEntity()`                                            | Create a new entity ID                                                                                                                                  | `Entity` (uint32_t)           |
|                          | `DestroyEntity(Entity entity)`                              | Destroy an entity and remove all its components                                                                                                         | `void`                        |
| **Component Operations** | `AddComponent<T>(Entity entity, const T& component)`        | Add a component to an entity. Auto-registers component type on first use. Updates entity signature and system membership.                               | `void`                        |
|                          | `RemoveComponent<T>(Entity entity)`                         | Remove a component from an entity. Updates entity signature and system membership.                                                                      | `void`                        |
|                          | `Get<T>(Entity entity)`                                     | Get a reference to an entity's component. **Crashes if component doesn't exist.**                                                                       | `T&`                          |
|                          | `GetOptional<T>(Entity entity)`                             | Safely get a pointer to an entity's component. Returns nullptr if it doesn't exist.                                                                     | `T*`                          |
| **System Management**    | `RegisterSystem<SystemType, Component1, Component2, ...>()` | Register a system and specify its required components. Auto-registers component types. System will process entities that have ALL specified components. | `std::shared_ptr<SystemType>` |

## Quick Start

Here's the typical workflow for using this ECS:

```cpp
// Get the ECS coordinator (singleton)
auto& coordinator = Engine::Coordinator::GetInstance();

// STEP 1: Register systems first (usually during initialization)
// Systems define what components they need to process
auto renderSystem = coordinator.RegisterSystem<RenderSystem, Transform, Sprite>();
auto movementSystem = coordinator.RegisterSystem<MovementSystem, Transform, Velocity>();

// Initialize systems with any resources they need
renderSystem->Init(renderer, textureManager);

// STEP 2: Create entities and add components (during gameplay/setup)
// Components are automatically registered on first use - no manual setup needed!
Entity player = coordinator.CreateEntity();

coordinator.AddComponent<Transform>(player, {
    .position = {100.0f, 100.0f},
    .rotation = 0.0f,
    .scale = {1.0f, 1.0f}
});

coordinator.AddComponent<Sprite>(player, {
    .textureId = TextureID::Player,
    .srcRect = {0, 0, 32, 32}
});

coordinator.AddComponent<Velocity>(player, {
    .velocity = {50.0f, 0.0f}
});

// Player is now automatically in both RenderSystem and MovementSystem!

// STEP 3: Update systems in your game loop
while (running) {
    movementSystem->Update(deltaTime);  // Processes all entities with Transform + Velocity
    renderSystem->Update();              // Renders all entities with Transform + Sprite
}
```

That's it! The ECS automatically:

- **Registers components** on first use (no manual `RegisterComponent` calls needed)
- Routes entities to the right systems based on their components
- Stores component data efficiently in packed arrays
- Updates system entity lists when components are added/removed

## Core Concepts

### Entities

Entities are just numeric IDs (`uint32_t`). They're lightweight handles that connect components together.

```cpp
Entity player = coordinator.CreateEntity();    // Get ID: 0
Entity enemy = coordinator.CreateEntity();     // Get ID: 1
coordinator.DestroyEntity(player);             // Recycle ID 0 for reuse
```

**Key features:**

- Max 1000 entities (configurable in `Types.h`)
- IDs are recycled when entities are destroyed
- Each entity has a signature (bitset) tracking which components it has

### Components

Components are pure data structs.

```cpp
struct Transform {
    SDL_FPoint position;
    float rotation;
    SDL_FPoint scale;
};

struct Velocity {
    SDL_FPoint velocity;
};

struct Health {
    float current;
    float max;
};
```

**Requirements:**

- Must be copyable and movable (usually automatic for simple structs)
- Keep them small and cache-friendly
- Avoid heavy types like `std::vector` or `std::unique_ptr` when possible

**Working with components:**

```cpp
// Add a component
coordinator.AddComponent<Transform>(entity, {.position = {0, 0}, ...});

// Get a component (crashes if doesn't exist)
Transform& t = coordinator.Get<Transform>(entity);
t.position.x += 10;

// Check if component exists, then get it (safe)
if (auto* health = coordinator.GetOptional<Health>(entity)) {
    health->current -= damage;
}

// Remove a component
coordinator.RemoveComponent<Velocity>(entity);
```

### Systems

Systems process entities that have specific components. They inherit from `Engine::System` and implement their own logic.

```cpp
class MovementSystem : public Engine::System {
public:
    void Update(float dt) {
        // _entities contains all entities with required components
        for (const auto& entity : _entities) {
            auto& transform = coordinator.Get<Transform>(entity);
            auto& velocity = coordinator.Get<Velocity>(entity);

            transform.position.x += velocity.velocity.x * dt;
            transform.position.y += velocity.velocity.y * dt;
        }
    }
};

// Register with required components (Transform + Velocity)
auto movementSystem = coordinator.RegisterSystem<MovementSystem, Transform, Velocity>();

// Call every frame
movementSystem->Update(deltaTime);
```

The ECS automatically maintains `_entities`—entities are added when they gain required components and removed when they lose them.

**Example: Render System**

```cpp
class RenderSystem : public Engine::System {
public:
    void Init(SDL_Renderer* renderer, TextureManager* texManager) {
        _renderer = renderer;
        _texManager = texManager;
    }

    void Update() {
        for (const auto& entity : _entities) {
            auto& transform = coordinator.Get<Transform>(entity);
            auto& sprite = coordinator.Get<Sprite>(entity);

            SDL_Texture* texture = _texManager->Get(sprite.textureId);

            SDL_FRect dstRect = {
                transform.position.x,
                transform.position.y,
                sprite.srcRect.w * transform.scale.x,
                sprite.srcRect.h * transform.scale.y
            };

            SDL_RenderTextureRotated(_renderer, texture, &sprite.srcRect,
                                     &dstRect, transform.rotation,
                                     nullptr, sprite.flipMode);
        }
    }

private:
    SDL_Renderer* _renderer;
    TextureManager* _texManager;
};
```

## Optional Components

`GetOptional` lets you safely query components that might not exist. This is crucial for building flexible systems.

### The Problem

Without optional queries, you'd need separate systems for every variation:

- ❌ `RenderSystem` (Transform + Sprite)
- ❌ `RenderWithHealthBarSystem` (Transform + Sprite + Health)
- ❌ `RenderWithGlowSystem` (Transform + Sprite + GlowEffect)
- ❌ `RenderWithHealthAndGlowSystem` (Transform + Sprite + Health + GlowEffect)

This explodes quickly as you add more optional features.

### The Solution

One system with optional checks:

```cpp
class RenderSystem : public Engine::System {
public:
    void Update() {
        for (const auto& entity : _entities) {
            auto& transform = coordinator.Get<Transform>(entity);
            auto& sprite = coordinator.Get<Sprite>(entity);

            // Render base sprite (required)
            DrawSprite(sprite, transform);

            // Optional: draw health bar if entity has Health
            if (auto* health = coordinator.GetOptional<Health>(entity)) {
                DrawHealthBar(transform, health->current, health->max);
            }

            // Optional: draw glow effect if entity has GlowEffect
            if (auto* glow = coordinator.GetOptional<GlowEffect>(entity)) {
                DrawGlow(transform, glow->color, glow->intensity);
            }

            // Optional: draw name tag if entity has Name
            if (auto* name = coordinator.GetOptional<Name>(entity)) {
                DrawText(name->text, transform.position);
            }
        }
    }
};

// Register with only required components
auto renderSystem = coordinator.RegisterSystem<RenderSystem, Transform, Sprite>();
```

Now entities can have any combination of optional visual features without needing new systems.

### Common Use Cases

**Physics with optional forces:**

```cpp
class PhysicsSystem : public Engine::System {
public:
    void Update(float dt) {
        for (const auto& entity : _entities) {
            auto& transform = coordinator.Get<Transform>(entity);
            auto& velocity = coordinator.Get<Velocity>(entity);

            // Optional gravity
            if (auto* gravity = coordinator.GetOptional<AffectedByGravity>(entity)) {
                velocity.velocity.y += gravity->force * dt;
            }

            // Optional friction (for ground entities)
            if (auto* friction = coordinator.GetOptional<Friction>(entity)) {
                velocity.velocity.x *= (1.0f - friction->coefficient * dt);
            }

            // Optional drag (for flying/swimming entities)
            if (auto* drag = coordinator.GetOptional<Drag>(entity)) {
                velocity.velocity *= (1.0f - drag->coefficient * dt);
            }

            // Apply velocity
            transform.position.x += velocity.velocity.x * dt;
            transform.position.y += velocity.velocity.y * dt;
        }
    }
};
```

**Combat with optional armor/shields:**

```cpp
void CombatSystem::ApplyDamage(Entity target, float baseDamage) {
    auto& health = coordinator.Get<Health>(target);
    float finalDamage = baseDamage;

    // Optional armor
    if (auto* armor = coordinator.GetOptional<Armor>(target)) {
        finalDamage *= (1.0f - armor->damageReduction);
    }

    // Optional shield (absorbs damage before health)
    if (auto* shield = coordinator.GetOptional<Shield>(target)) {
        float absorbed = std::min(finalDamage, shield->current);
        shield->current -= absorbed;
        finalDamage -= absorbed;
    }

    health.current -= finalDamage;
}
```

**AI with optional behaviors:**

```cpp
class AISystem : public Engine::System {
public:
    void Update(float dt) {
        for (const auto& entity : _entities) {
            auto& ai = coordinator.Get<AIState>(entity);

            // Optional patrol path
            if (auto* patrol = coordinator.GetOptional<PatrolPath>(entity)) {
                FollowPatrolPath(entity, patrol);
            }

            // Optional aggro detection
            if (auto* aggro = coordinator.GetOptional<AggroRange>(entity)) {
                if (PlayerInRange(entity, aggro->range)) {
                    ChasePlayer(entity);
                }
            }

            // Optional flee behavior when low health
            if (auto* health = coordinator.GetOptional<Health>(entity)) {
                if (health->current < health->max * 0.2f) {
                    FleeFromPlayer(entity);
                }
            }
        }
    }
};
```

### How It Works

```cpp
template <typename Component>
Component* GetOptional(Entity entity) const {
    ComponentType componentType = GetComponentType<Component>();

    // Fast bitset check first (O(1))
    if (_entityManager->GetSignature(entity).test(componentType)) {
        return &_componentManager->GetComponent<Component>(entity);
    }
    return nullptr;
}
```

It checks the entity's signature (which components it has) before attempting retrieval. This is fast and safe—no exceptions, no crashes.

## Under the Hood

### Component Storage

Components are stored in packed arrays, one array per component type:

```
Transform Array: [T0][T1][T2][T3]... (contiguous memory)
Velocity Array:  [V0][V1][V2][V3]... (contiguous memory)
Health Array:    [H0][H1][H2]...     (contiguous memory)
```

This is **data-oriented design**—when systems iterate entities, they access components sequentially in memory, maximizing cache hits.

**ComponentArray structure:**

```cpp
template <typename T>
class ComponentArray {
    std::array<T, MAX_COMPONENTS> _componentArray;       // Packed data
    std::unordered_map<Entity, size_t> _entityToIndex;  // Entity → Array index
    std::unordered_map<size_t, Entity> _indexToEntity;  // Array index → Entity
    size_t _size;                                       // Number of active components
};
```

**Adding a component:**

1. Store data at `_componentArray[_size]`
2. Map entity ID to array index
3. Increment `_size`

**Removing a component (swap-and-pop):**

1. Copy last element into removed element's spot
2. Update maps for the moved element
3. Decrement `_size`

This keeps the array packed with no gaps (fast iteration) and removal is O(1).

### Signatures and System Matching

Each entity has a **signature** (a `std::bitset<32>`) where each bit represents a component type:

```
Entity 0: 00000111  (has Transform, Velocity, Sprite)
Entity 1: 00000011  (has Transform, Velocity)
Entity 2: 00000101  (has Transform, Sprite)
```

When you register a system, it gets a signature for its required components:

```cpp
// RenderSystem requires Transform + Sprite
auto renderSystem = coordinator.RegisterSystem<RenderSystem, Transform, Sprite>();
// System signature: 00000101
```

**Automatic matching:**

When an entity's signature changes (component added/removed), the SystemManager checks all systems:

```cpp
void EntitySignatureChanged(Entity entity, Signature entitySignature) {
    for (auto& [type, system] : _systems) {
        Signature systemSignature = _signatures[type];

        // Bitwise AND: does entity have ALL required components?
        if ((entitySignature & systemSignature) == systemSignature) {
            system->_entities.insert(entity);  // Add to system
        } else {
            system->_entities.erase(entity);   // Remove from system
        }
    }
}
```

This is extremely fast—just bitset operations.

### Automatic Registration

Components are automatically registered the first time they're used:

```cpp
coordinator.AddComponent<Transform>(entity, {...});
// If Transform hasn't been seen before:
// 1. Assigns it a unique ComponentType ID (0, 1, 2, ...)
// 2. Creates a ComponentArray<Transform> to store all Transforms
```

No manual registration needed. Systems also trigger component registration when you specify their required components.

## Complete Example

Here's a full game setup:

```cpp
// Define components
struct Transform {
    SDL_FPoint position;
    float rotation;
    SDL_FPoint scale;
};

struct Sprite {
    TextureID textureId;
    SDL_FRect srcRect;
};

struct Velocity {
    SDL_FPoint velocity;
};

struct Health {
    float current;
    float max;
};

// Create systems (shown earlier)
// - MovementSystem
// - RenderSystem
// - CombatSystem

// Initialize ECS
void Game::InitECS() {
    auto& coordinator = Engine::Coordinator::GetInstance();

    // Register systems
    _movementSystem = coordinator.RegisterSystem<MovementSystem, Transform, Velocity>();
    _renderSystem = coordinator.RegisterSystem<RenderSystem, Transform, Sprite>();
    _renderSystem->Init(_renderer, _textureManager);
}

// Spawn entities
void Game::SpawnPlayer() {
    Entity player = coordinator.CreateEntity();

    coordinator.AddComponent<Transform>(player, {
        .position = {400, 300},
        .rotation = 0,
        .scale = {2, 2}
    });

    coordinator.AddComponent<Sprite>(player, {
        .textureId = TextureID::Player,
        .srcRect = {0, 0, 32, 32}
    });

    coordinator.AddComponent<Velocity>(player, {
        .velocity = {0, 0}
    });

    coordinator.AddComponent<Health>(player, {
        .current = 100,
        .max = 100
    });
}

void Game::SpawnEnemy(SDL_FPoint position) {
    Entity enemy = coordinator.CreateEntity();

    coordinator.AddComponent<Transform>(enemy, {
        .position = position,
        .rotation = 0,
        .scale = {1, 1}
    });

    coordinator.AddComponent<Sprite>(enemy, {
        .textureId = TextureID::Enemy,
        .srcRect = {0, 0, 32, 32}
    });

    coordinator.AddComponent<Health>(enemy, {
        .current = 50,
        .max = 50
    });

    // Note: no Velocity component, so MovementSystem won't process it
}

// Game loop
void Game::Run() {
    while (_running) {
        float deltaTime = CalculateDeltaTime();

        HandleInput();
        Update(deltaTime);
        Render();
    }
}

void Game::Update(float deltaTime) {
    _movementSystem->Update(deltaTime);
    // Other systems...
}

void Game::Render() {
    SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
    SDL_RenderClear(_renderer);

    _renderSystem->Update();

    SDL_RenderPresent(_renderer);
}
```

### System Base Class

```cpp
class System {
public:
    std::set<Entity> _entities;  // Automatically managed
};
```

All your systems inherit from this and implement their own `Update()` or other methods.

## Performance Notes

**Current limits** (configurable in `Types.h`):

- Max 1000 entities (`MAX_ENTITIES`)
- Max 32 component types (bitset size)

**Why it's fast:**

- **Packed arrays**: Components stored contiguously in memory
- **Cache-friendly**: Iterating entities accesses sequential memory
- **Bitset matching**: Entity-to-system matching is O(1) bitwise operations
- **No virtual calls**: Components are plain structs

## Building

```bash
mkdir build && cd build
cmake ..
make
./Top-Down-Shooter
```

## Future Work

Planned systems:

- Input system
- Collision system
- Weapon/shooting system
- Enemy AI system
- Particle system
- Audio system

## Acknowledgments

This ECS was heavily inspired by:

- [Austin Morlan's ECS article](https://austinmorlan.com/posts/entity_component_system/) for the core architecture
- Modern ECS libraries for best practices and API design

This project is a learning exercise features are added as they're needed for the main game.