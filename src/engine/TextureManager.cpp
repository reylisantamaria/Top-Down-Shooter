#include "engine/TextureManager.h"
#include <iostream>

Engine::TextureManager &Engine::TextureManager::GetInstance()
{
  static Engine::TextureManager instance;
  return instance;
}

Engine::TextureManager::~TextureManager()
{
  Clear();
}

void Engine::TextureManager::Init(SDL_Renderer *renderer)
{
  if (!renderer)
  {
    std::cerr << "TextureManager::Init - Invalid renderer provided\n";
    return;
  }
  _renderer = renderer;
}

void Engine::TextureManager::LoadAllTextures()
{
  for (const auto &[id, filepath] : TextureAssets)
  {
    Load(id, filepath);
  }
}

bool Engine::TextureManager::Load(TextureID id, const std::string &filepath)
{
  // Validate renderer is initialized
  if (!_renderer)
  {
    std::cerr << "TextureManager::Load - TextureManager not initialized (call Init first)\n";
    return false;
  }

  // Skip if already loaded (not an error)
  if (_textures.find(id) != _textures.end())
  {
    return true;
  }

  // Load image from disk into surface
  SDL_Surface *surface = IMG_Load(filepath.c_str());
  if (!surface)
  {
    std::cerr << "TextureManager::Load - Failed to load image '" << filepath
              << "': " << SDL_GetError() << "\n";
    return false;
  }

  // Convert surface to GPU texture
  SDL_Texture *texture = SDL_CreateTextureFromSurface(_renderer, surface);
  SDL_DestroySurface(surface); // Surface no longer needed

  if (!texture)
  {
    std::cerr << "TextureManager::Load - Failed to create texture from '" << filepath
              << "': " << SDL_GetError() << "\n";
    return false;
  }

  // Set default scale mode for pixel art (can be overridden per-sprite if needed)
  SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

  // Store texture for future retrieval
  _textures[id] = texture;
  return true;
}

SDL_Texture *Engine::TextureManager::Get(TextureID id) const
{
  auto itr = _textures.find(id);
  return (itr != _textures.end()) ? itr->second : nullptr;
}

void Engine::TextureManager::Unload(TextureID id)
{
  auto itr = _textures.find(id);
  if (itr != _textures.end())
  {
    SDL_DestroyTexture(itr->second);
    _textures.erase(itr);
  }
}

void Engine::TextureManager::Clear()
{
  for (const auto &[id, texture] : _textures)
  {
    if (texture)
    {
      SDL_DestroyTexture(texture);
    }
  }
  _textures.clear();
}
