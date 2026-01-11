#pragma once

#include "engine/Coordinator.h"
#include "engine/Vec2.h"
#include "game/Components.h"
#include "game/TextureAssets.h"

// EntityCreator keeps helpers to spawn common game entities.
namespace EntityCreator
{
  // PlayerConfig keeps the basic values for creating the player.
  struct PlayerConfig
  {
    Engine::Vec2 position{0.0f, 0.0f};
    float speed{300.0f};
  };

  // CreatePlayer sets up everything the player needs.
  inline Engine::Entity CreatePlayer(const PlayerConfig &config = {})
  {
    auto &coordinator = Engine::Coordinator::GetInstance();
    Engine::Entity player = coordinator.CreateEntity();

    coordinator.AddComponent<Components::Transform>(player, {.position = config.position});
    coordinator.AddComponent<Components::Sprite>(player, {.textureId = TextureID::Player,
                                                          .srcRect = {0.0f, 0.0f, 64.0f, 64.0f},
                                                          .scaleMode = SDL_SCALEMODE_NEAREST,
                                                          .flipMode = SDL_FLIP_NONE,
                                                          .pivotPoint = {32.0f, 32.0f}});

    coordinator.AddComponent<Components::Speed>(player, {.value = config.speed});
    coordinator.AddComponent<Components::Velocity>(player, {});

    coordinator.AddComponent<Components::AimIntent>(player, {});
    coordinator.AddComponent<Components::FireIntent>(player, {});
    coordinator.AddComponent<Components::MoveIntent>(player, {});

    coordinator.AddComponent<Components::Player>(player, {});

    return player;
  }

  // CreateLaserWeapon builds a laser weapon tied to the owner.
  inline Engine::Entity CreateLaserWeapon(Engine::Entity owner)
  {
    auto &coordinator = Engine::Coordinator::GetInstance();
    Engine::Entity weapon = coordinator.CreateEntity();

    // Weapon stats keep firing and visual values together.
    coordinator.AddComponent<Components::WeaponStats>(weapon, {.fireRate = 0.2f,           // 10 shots per second
                                                               .projectileSpeed = 600.0f,  // Fast projectiles
                                                               .projectileDamage = 10.0f,  // Damage per hit
                                                               .projectileLifetime = 2.0f, // Lives for 2 seconds
                                                               .projectileOffset = 50.0f,  // Offset from owner center (pixels)
                                                               .projectileTexture = TextureID::LaserBeam,
                                                               .projectileSrcRect = {0.0f, 0.0f, 16.0f, 16.0f},
                                                               .projectilePivotPoint = {8.0f, 8.0f}});

    // Cooldown starts at zero so it can fire immediately.
    coordinator.AddComponent<Components::Cooldown>(weapon, {.remaining = 0.0f});

    // OwnedBy links the weapon back to its owner.
    coordinator.AddComponent<Components::OwnedBy>(weapon, {.owner = owner});

    // Tag the entity as a weapon.
    coordinator.AddComponent<Components::Weapon>(weapon, {});

    return weapon;
  }

  // CreateProjectile wires up a projectile with everything it needs.
  inline Engine::Entity CreateProjectile(const Engine::Vec2 &position,
                                         const Engine::Vec2 &direction,
                                         const Components::WeaponStats &weaponStats,
                                         const float rotation)
  {
    auto &coordinator = Engine::Coordinator::GetInstance();
    Engine::Entity projectile = coordinator.CreateEntity();

    coordinator.AddComponent<Components::Transform>(projectile, {.position = position, .rotation = rotation});

    coordinator.AddComponent<Components::Velocity>(projectile, {.vector = direction * weaponStats.projectileSpeed});

    coordinator.AddComponent<Components::Damage>(projectile, {.value = weaponStats.projectileDamage});

    coordinator.AddComponent<Components::Lifetime>(projectile, {.remaining = weaponStats.projectileLifetime});

    coordinator.AddComponent<Components::Sprite>(projectile, {.textureId = weaponStats.projectileTexture,
                                                              .srcRect = weaponStats.projectileSrcRect,
                                                              .scaleMode = SDL_SCALEMODE_NEAREST,
                                                              .flipMode = SDL_FLIP_NONE,
                                                              .pivotPoint = weaponStats.projectilePivotPoint});

    return projectile;
  }
}
