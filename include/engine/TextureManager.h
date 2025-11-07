#pragma once

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <unordered_map>
#include <string>

#include "game/TextureAssets.h"

namespace Engine
{
  /**
   * TextureManager - Singleton class for managing game textures
   * 
   * Handles loading, storing, and retrieving SDL textures. Automatically
   * cleans up all textures on destruction.
   * 
   * Usage:
   *   auto& texManager = TextureManager::GetInstance();
   *   texManager.Init(renderer);
   *   texManager.LoadAllTextures();
   *   SDL_Texture* tex = texManager.Get(TextureID::Player);
   */
  class TextureManager
  {
  public:
    // Get the singleton instance
    static TextureManager &GetInstance();

    // Initialize with SDL renderer (required before loading textures)
    void Init(SDL_Renderer *renderer);

    // Load all textures defined in TextureAssets map
    void LoadAllTextures();

    // Load a single texture from file path
    // Returns true if successful or already loaded, false on error
    // Requires Init() to be called first
    bool Load(TextureID id, const std::string &filepath);

    // Get a loaded texture by ID
    // Returns texture pointer or nullptr if not found
    SDL_Texture *Get(TextureID id) const;

    // Remove a specific texture from memory
    void Unload(TextureID id);

    // Clear all textures from memory (called automatically in destructor)
    void Clear();

  private:
    TextureManager() = default;
    ~TextureManager();

    // Prevent copying
    TextureManager(const TextureManager &) = delete;
    TextureManager &operator=(const TextureManager &) = delete;

    SDL_Renderer *_renderer = nullptr;
    std::unordered_map<TextureID, SDL_Texture *> _textures;
  };

}