#include "game/Systems.h"
#include <iostream>

extern Engine::Coordinator _coordinator;

// =============================================================================
// Render System Implementation
// =============================================================================
void Systems::RenderSystem::Init(SDL_Renderer *renderer, Engine::TextureManager *textureManager)
{
  _renderer = renderer;
  _textureManager = textureManager;
}

void Systems::RenderSystem::Update()
{
  // Iterate through all entities that have Transform and Sprite components
  for (const auto &entity : _entitiesSet)
  {
    auto &transform = _coordinator.GetComponent<Components::Transform>(entity);
    auto &sprite = _coordinator.GetComponent<Components::Sprite>(entity);

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
