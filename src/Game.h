#pragma once

#include <SDL3/SDL.h>
#include <memory>
#include "engine/Coordinator.h"
#include "game/Systems.h"
#include "game/Components.h"

class Game
{
public:
  Game() = default;
  ~Game();
  bool Init();
  void Run();

private:
  // Initialization
  bool InitSDL();
  bool InitECS();
  bool LoadAssets();

  // Game loop methods
  void HandleEvents();
  void Update(float deltaTime);
  void Render() const;
  void Cleanup();
  
  // SDL members
  SDL_Window *_window = nullptr;
  SDL_Renderer *_renderer = nullptr;
  bool _running = false;

  std::shared_ptr<Systems::RenderSystem> _renderSystem;
  std::shared_ptr<Systems::InputSystem> _inputSystem;
  std::shared_ptr<Systems::MovementSystem> _movementSystem;
  std::shared_ptr<Systems::AimSystem> _aimSystem;

  
  Engine::Entity _playerEntity;
};