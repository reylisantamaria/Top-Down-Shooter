#include "game/Systems.h"
#include <iostream>

inline Engine::Vec2 GetSpriteCenter(const Engine::Entity &entity)
{
  auto &coordinator = Engine::Coordinator::GetInstance();
  auto *transform = coordinator.GetOptional<Components::Transform>(entity);
  auto *sprite = coordinator.GetOptional<Components::Sprite>(entity);

  if (transform && sprite)
  {
    return transform->position + Engine::Vec2{sprite->pivotPoint.x * transform->scale.x, sprite->pivotPoint.y * transform->scale.y};
  }

  return transform->position;
}

void Systems::RenderSystem::Init(SDL_Renderer *renderer, Engine::TextureManager *textureManager)
{
  _renderer = renderer;
  _textureManager = textureManager;
}

void Systems::RenderSystem::Update()
{
  auto &coordinator = Engine::Coordinator::GetInstance();

  // Iterate through all entities that have Transform and Sprite components
  for (const auto &entity : _entities)
  {
    auto &transform = coordinator.Get<Components::Transform>(entity);
    auto &sprite = coordinator.Get<Components::Sprite>(entity);
    auto *texture = _textureManager->Get(sprite.textureId);

    // Figure out where to draw the sprite and how big it should be.
    SDL_FRect dstRect;
    dstRect.x = transform.position.x;
    dstRect.y = transform.position.y;
    dstRect.w = sprite.srcRect.w * transform.scale.x;
    dstRect.h = sprite.srcRect.h * transform.scale.y;

    // Render it with its rotation and any flip that sprite needs.
    SDL_RenderTextureRotated(_renderer, texture, &sprite.srcRect, &dstRect,
                             transform.rotation, &sprite.pivotPoint, sprite.flipMode);
  }
}

void Systems::PlayerInputSystem::Update()
{
  auto keyboardState = SDL_GetKeyboardState(nullptr);

  float mouseX, mouseY;
  auto mouseState = SDL_GetMouseState(&mouseX, &mouseY);

  auto &coordinator = Engine::Coordinator::GetInstance();

  for (const auto &entity : _entities)
  {
    auto &moveIntent = coordinator.Get<Components::MoveIntent>(entity);
    auto &aimIntent = coordinator.Get<Components::AimIntent>(entity);
    auto &fireIntent = coordinator.Get<Components::FireIntent>(entity);

    // Reset the movement direction before checking the keys.
    moveIntent.direction = {0.0f, 0.0f};

    if (keyboardState[SDL_SCANCODE_W])
    {
      moveIntent.direction += Directions::UP;
    }
    if (keyboardState[SDL_SCANCODE_S])
    {
      moveIntent.direction += Directions::DOWN;
    }
    if (keyboardState[SDL_SCANCODE_A])
    {
      moveIntent.direction += Directions::LEFT;
    }
    if (keyboardState[SDL_SCANCODE_D])
    {
      moveIntent.direction += Directions::RIGHT;
    }

    // Normalize so diagonal feels as fast as moving straight.
    moveIntent.direction.normalize();

    aimIntent.target = {mouseX, mouseY};
    // Fire intent is on when it detects the left mouse button.
    fireIntent.active = (mouseState & SDL_BUTTON_LMASK);
  }
}

void Systems::VelocitySystem::Update()
{
  auto &coordinator = Engine::Coordinator::GetInstance();

  for (const auto &entity : _entities)
  {
    auto &moveIntent = coordinator.Get<Components::MoveIntent>(entity);
    auto &velocity = coordinator.Get<Components::Velocity>(entity);
    auto &speed = coordinator.Get<Components::Speed>(entity);

    velocity.vector = moveIntent.direction * speed.value;
  }
}

void Systems::MovementSystem::Update(float dt)
{
  auto &coordinator = Engine::Coordinator::GetInstance();

  for (const auto &entity : _entities)
  {
    auto &transform = coordinator.Get<Components::Transform>(entity);
    auto &velocity = coordinator.Get<Components::Velocity>(entity);

    // Move the transform by the current velocity scaled by delta time.
    transform.position += velocity.vector * dt;
  }
}

void Systems::AimSystem::Update()
{
  auto &coordinator = Engine::Coordinator::GetInstance();

  constexpr float kSpriteFacingOffsetDegrees = 90.0f; // Sprite faces right by default, so rotate +90 degrees.
  constexpr float kRadiansToDegrees = 180.0f / M_PI;  // Convert SDL angles from radians to degrees.

  for (const auto &entity : _entities)
  {
    auto &transform = coordinator.Get<Components::Transform>(entity);
    auto &aimIntent = coordinator.Get<Components::AimIntent>(entity);

    Engine::Vec2 entityCenter = GetSpriteCenter(entity);

    // Compute the direction from the sprite's center to the cursor.
    aimIntent.direction = aimIntent.target - entityCenter;

    auto angleInDegrees = std::atan2(aimIntent.direction.y, aimIntent.direction.x) * kRadiansToDegrees;
    transform.rotation = angleInDegrees;
  }
}

void Systems::WeaponSystem::Update()
{
  auto &coordinator = Engine::Coordinator::GetInstance();

  for (const auto &entity : _entities)
  {
    auto &ownedBy = coordinator.Get<Components::OwnedBy>(entity);
    // Grab the fire intent from whoever owns this weapon.
    auto &fireIntent = coordinator.Get<Components::FireIntent>(ownedBy.owner);
    // Cooldown that keeps the weapon from firing until it runs out.
    auto &cooldown = coordinator.Get<Components::Cooldown>(entity);

    if (cooldown.remaining > 0.0f)
      continue;

    // Skip unless the owner actually wants to fire.
    if (fireIntent.active)
    {
      auto &weaponStats = coordinator.Get<Components::WeaponStats>(entity);
      // Owner transform keeps the projectile's rotation aligned.
      auto &transform = coordinator.Get<Components::Transform>(ownedBy.owner);
      // Aim direction used when spawning the projectile.
      auto &aimIntent = coordinator.Get<Components::AimIntent>(ownedBy.owner);

      if (aimIntent.direction.normalize())
      {
        // Figure out the spawn position from the owner's center and aim.
        auto entityCenter = GetSpriteCenter(ownedBy.owner);
        auto projectilePosition = entityCenter + (aimIntent.direction * weaponStats.projectileOffset);

        EntityCreator::CreateProjectile(projectilePosition, aimIntent.direction, weaponStats, transform.rotation);

        // Reset the cooldown so it won't fire again immediately after.
        cooldown.remaining = weaponStats.fireRate;
      }
      else
      {
        std::cerr << "Invalid aim direction for weapon " << entity << std::endl;
      }
    }
  }
}

void Systems::CooldownSystem::Update(float dt)
{
  auto &coordinator = Engine::Coordinator::GetInstance();

  // Knock down each cooldown timer and clamp it at zero.
  for (const auto &entity : _entities)
  {
    auto &cooldown = coordinator.Get<Components::Cooldown>(entity);

    cooldown.remaining -= dt;
    cooldown.remaining = std::max(0.0f, cooldown.remaining);
  }
}

void Systems::LifetimeSystem::Update(float dt)
{
  auto &coordinator = Engine::Coordinator::GetInstance();

  // Collect the entities that should disappear this frame.
  std::vector<Engine::Entity> entitiesToDestroy;

  for (const auto &entity : _entities)
  {
    auto &lifetime = coordinator.Get<Components::Lifetime>(entity);

    lifetime.remaining -= dt;
    lifetime.remaining = std::max(0.0f, lifetime.remaining);

    if (lifetime.remaining == 0.0f)
    {
      entitiesToDestroy.push_back(entity);
    }
  }

  // Destroy them afterward so no loop runs into invalid handles.
  for (const auto &entity : entitiesToDestroy)
  {
    coordinator.DestroyEntity(entity);
  }
}
