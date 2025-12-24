# ECS Engine

A custom Entity Component System (ECS) implementation for a 2D top-down shooter using SDL3 and C++.

This is my take on an ECS. It uses packed arrays for cache-friendly performance and automatic signature-based entity-to-system matching. This is overkill for a casual game like this, but I wanted to challenge myself to understand how ECS works.

## How to Use

All ECS operations go through the `Coordinator` singleton:

```cpp
auto& coordinator = Engine::Coordinator::GetInstance();
```

### Basic Workflow

```cpp
// 1. Register systems (tell them what components they need)
auto renderSystem = coordinator.RegisterSystem<RenderSystem, Transform, Sprite>();
auto movementSystem = coordinator.RegisterSystem<MovementSystem, Transform, Velocity>();

// 2. Create entities and add components
Entity player = coordinator.CreateEntity();
coordinator.AddComponent<Transform>(player, {.position = {100, 100}});
coordinator.AddComponent<Sprite>(player, {.textureId = TextureID::Player});
coordinator.AddComponent<Velocity>(player, {.velocity = {50, 0}});

// 3. Update systems each frame
movementSystem->Update(deltaTime);
renderSystem->Update();
```

The ECS automatically sends entities to the right systems based on their components.

## Key Concepts

### Entities

Entities are just IDs that group components together.

```cpp
Entity player = coordinator.CreateEntity();
coordinator.DestroyEntity(player);
```

### Components

Components are plain data structs:

```cpp
struct Transform {
    Engine::Vec2 position;          // World position (x, y)
    float rotation{0.0f};           // Rotation in degrees (0-360)
    Engine::Vec2 scale{1.0f, 1.0f}; // Scale factor (x, y)
};

struct Velocity {
    Engine::Vec2 vel;
    float speed;
};
```

Add, get, or remove them:

```cpp
// Add
coordinator.AddComponent<Transform>(entity, {.position = {0, 0}});

// Get
Transform& t = coordinator.Get<Transform>(entity);
t.position.x += 10;

// Get safely (returns nullptr if missing)
if (auto* health = coordinator.GetOptional<Health>(entity)) {
    health->current -= damage;
}

// Remove
coordinator.RemoveComponent<Velocity>(entity);
```

### Systems

Systems process entities that have certain components. Inherit from `Engine::System`:

```cpp
class MovementSystem : public Engine::System {
public:
    void Update(float dt) {
        for (const auto& entity : _entities) {
            auto& transform = coordinator.Get<Transform>(entity);
            auto& velocity = coordinator.Get<Velocity>(entity);

            transform.position += velocity.vel * dt;
        }
    }
};
```

Register it with the components it needs:

```cpp
auto movementSystem = coordinator.RegisterSystem<MovementSystem, Transform, Velocity>();
```

The `_entities` set is automatically updated when components are added or removed.

## Optional Components

Use `GetOptional` to check for components that might not exist:

```cpp
class RenderSystem : public Engine::System {
public:
    void Update() {
        for (const auto& entity : _entities) {
            auto& transform = coordinator.Get<Transform>(entity);
            auto& sprite = coordinator.Get<Sprite>(entity);

            DrawSprite(sprite, transform);

            // Draw health bar only if entity has Health component
            if (auto* health = coordinator.GetOptional<Health>(entity)) {
                DrawHealthBar(transform, health->current, health->max);
            }
        }
    }
};
```

This lets one system handle many variations without creating separate systems for each combination.

## Complete Example

```cpp
// Define components
struct Transform {
    Engine::Vec2 position;          // World position (x, y)
    float rotation{0.0f};           // Rotation in degrees (0-360)
    Engine::Vec2 scale{1.0f, 1.0f}; // Scale factor (x, y)
};

struct Sprite {
    TextureID textureId;
    SDL_FRect srcRect;
};

struct Velocity {
    Engine::Vec2 vel;
    float speed;
};

// Create a system
class MovementSystem : public Engine::System {
public:
    void Update(float dt) {
        for (const auto& entity : _entities) {
            auto& transform = coordinator.Get<Transform>(entity);
            auto& velocity = coordinator.Get<Velocity>(entity);

            transform.position += velocity.vel * dt;
        }
    }
};

// Set up in your game
void Game::Init() {
    auto& coordinator = Engine::Coordinator::GetInstance();

    // Register systems
    _movementSystem = coordinator.RegisterSystem<MovementSystem, Transform, Velocity>();
    _renderSystem = coordinator.RegisterSystem<RenderSystem, Transform, Sprite>();

    // Create player
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
        .vel = {0, 0},
        .speed = 300.0f
    });
}

// Update in your game loop
void Game::Update(float deltaTime) {
    _movementSystem->Update(deltaTime);
}

void Game::Render() {
    _renderSystem->Update();
}
```

## Quick Reference

### Coordinator Functions

| Function | What It Does |
| -------- | ------------ |
| `CreateEntity()` | Create a new entity |
| `DestroyEntity(entity)` | Delete an entity and all its components |
| `GetEntityCount()` | Get the number of active entities |
| `AddComponent<T>(entity, data)` | Give an entity a component |
| `RemoveComponent<T>(entity)` | Take away a component |
| `Get<T>(entity)` | Get a component (crashes if missing) |
| `GetOptional<T>(entity)` | Get a component (returns nullptr if missing) |
| `RegisterSystem<System, Components...>()` | Create a system that needs certain components |

### Vec2 (2D Vector)

| Operation | Example | Result |
| --------- | ------- | ------ |
| Create | `Vec2 v{3, 4}` | `{3, 4}` |
| Add | `v1 + v2` or `v1 += v2` | Component-wise addition |
| Multiply (vector) | `v1 * v2` or `v1 *= v2` | Component-wise multiply |
| Multiply (scalar) | `v * 2.0f` or `v *= 2.0f` | Scale the vector |
| Length | `v.length()` | `5.0f` for `{3, 4}` |
| Normalize | `v.normalize()` | Makes length = 1, returns `false` if zero |

### Direction Constants

| Constant | Value |
| -------- | ----- |
| `Directions::UP` | `{0, -1}` |
| `Directions::DOWN` | `{0, 1}` |
| `Directions::LEFT` | `{-1, 0}` |
| `Directions::RIGHT` | `{1, 0}` |

## Limits

- Max 1000 entities (can change in `Types.h`)
- Max 32 component types
- Components must be simple structs (copyable/movable)
- Systems inherit from `Engine::System` and use the `_entities` set

Heavily Inspired:

- [Austin Morlan's ECS article](https://austinmorlan.com/posts/entity_component_system/) for the core architecture
- Modern ECS libraries for best practices and API design
