#include "game/Systems.h"
#include <iostream>
#include <string>

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

    if (!texture)
    {
      std::cerr << "RenderSystem::Update - Texture not found for entity " << entity << "\n";
      continue;
    }

    SDL_SetTextureScaleMode(texture, sprite.scaleMode);

    // Calculate destination rectangle (centered on position)
    SDL_FRect dstRect;
    dstRect.x = transform.position.x;
    dstRect.y = transform.position.y;
    dstRect.w = sprite.srcRect.w * transform.scale.x;
    dstRect.h = sprite.srcRect.h * transform.scale.y;

    // Render the texture
    SDL_RenderTextureRotated(_renderer, texture, &sprite.srcRect, &dstRect,
                             transform.rotation, nullptr, sprite.flipMode);
  }
}

/*
 * Input System
 * Handles player input
 */
void Systems::InputSystem::Update(const bool *keyboardState)
{

  auto &coordinator = Engine::Coordinator::GetInstance();

  for (const auto &entity : _entities)
  {
    auto &v = coordinator.Get<Components::Velocity>(entity);
    auto &d = coordinator.Get<Components::Direction>(entity);

    v.vel = {0.0f, 0.0f};                         // Reset the velocity to 0
    Engine::Vec2 temp_dir = {0.0f, 0.0f};         // Resets direction & enables accumulation of directions (for diagonal movement)

    /*
     * All this does is set the velocity based on the direction and the speed.
     * no movement is done here.
     *
     * The Direction component is used to store the direction of the player.
     */

    if (keyboardState[SDL_SCANCODE_W])
    {
      temp_dir += Directions::UP;
    }
    if (keyboardState[SDL_SCANCODE_S])
    {
      temp_dir += Directions::DOWN;
    }
    if (keyboardState[SDL_SCANCODE_A])
    {
      temp_dir += Directions::LEFT;
    }
    if (keyboardState[SDL_SCANCODE_D])
    {
      temp_dir += Directions::RIGHT;
    }

    if (temp_dir.normalize())                     // normalize returns false if zero-length (entity not moving)
    {
      d.dir = temp_dir;
      v.vel = temp_dir * v.speed;
      std::println("Direction: {:.2f}, {:.2f}", temp_dir.x, temp_dir.y);
    }
  }
}

/*
 * Movement System
 * Handles player movement
 */
void Systems::MovementSystem::Update(float dt)
{
  auto &coordinator = Engine::Coordinator::GetInstance();

  for (const auto &entity : _entities)
  {
    auto &t = coordinator.Get<Components::Transform>(entity);
    auto &v = coordinator.Get<Components::Velocity>(entity);

    // This updates the entity's position based on it's velocity
    t.position += v.vel * dt;
  }
}