#include "game/Systems.h"
#include <iostream>
#include <cmath>

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

    SDL_SetTextureScaleMode(texture, sprite.scaleMode);                       // Change accordingly to the purpose of the texture

    // Calculate where to draw the texture on the screen and how big it should be (dimensions)
    SDL_FRect dstRect;
    dstRect.x = transform.position.x;
    dstRect.y = transform.position.y;
    dstRect.w = sprite.srcRect.w * transform.scale.x;
    dstRect.h = sprite.srcRect.h * transform.scale.y;

    // Render the texture
    SDL_RenderTextureRotated(_renderer, texture, &sprite.srcRect, &dstRect,
                             transform.rotation, nullptr, sprite.flipMode);    // Using SDL_RenderTextureRotated to rotate the texture and flip it if needed
  }
}

/*
 * Input System
 * Handles player keyboard input (WASD) and sets velocity accordingly.
 */
void Systems::InputSystem::Update()
{
  auto keyboardState = SDL_GetKeyboardState(NULL);

  auto &coordinator = Engine::Coordinator::GetInstance();

  for (const auto &entity : _entities)
  {
    auto &velocity = coordinator.Get<Components::Velocity>(entity);
    auto &direction = coordinator.Get<Components::Direction>(entity);

    velocity.value = {0.0f, 0.0f};                                              // Reset the velocity to 0
    Engine::Vec2 temp_dir = {0.0f, 0.0f};                                       // Resets direction & enables accumulation of directions (for diagonal movement)

    // All this does is set the velocity based on the direction and the speed.
    // no movement is done here.
    // The Direction component is used to store the direction of the player.

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

    if (temp_dir.normalize())                                                   // normalize returns false if zero-length (entity not moving)
    {
      direction.value = temp_dir;
      velocity.value = temp_dir * velocity.speed;
    }
  }
}

/*
 * Movement System
 * Moves all entities based on their velocity.
 */
void Systems::MovementSystem::Update(float dt)
{
  auto &coordinator = Engine::Coordinator::GetInstance();

  for (const auto &entity : _entities)
  {
    auto &transform = coordinator.Get<Components::Transform>(entity);
    auto &velocity = coordinator.Get<Components::Velocity>(entity);

    transform.position += velocity.value * dt;                                  // Update position based on velocity
  }
}

/*
 * Aim System
 * Rotates entities to face the mouse cursor
 */
void Systems::AimSystem::Update()
{
  auto &coordinator = Engine::Coordinator::GetInstance();

  float mouseX, mouseY;
  SDL_GetMouseState(&mouseX, &mouseY);

  float offset = 90.0f;                                                       // Add 90 degrees b/c the sprite faces "up" by default (and not "right")
  float conversionFactor = 180.0f / M_PI;                                     // Convert radians to degrees since SDL uses degrees

  for (const auto &entity : _entities)
  {
    auto &transform = coordinator.Get<Components::Transform>(entity);

    // Calculate the difference in x and y between the mouse and the entity
    float dx = mouseX - transform.position.x;
    float dy = mouseY - transform.position.y;

    float angleInRadians = std::atan2(dy, dx);
    float angleInDegrees = angleInRadians * conversionFactor + offset;

    transform.rotation = angleInDegrees;                                        // rotation is handled in the render system
  }
}