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

  // Systems
  std::shared_ptr<Systems::PlayerInputSystem> _playerInputSystem;
  std::shared_ptr<Systems::CooldownSystem> _cooldownSystem;
  std::shared_ptr<Systems::LifetimeSystem> _lifetimeSystem;
  std::shared_ptr<Systems::AimSystem> _aimSystem;
  std::shared_ptr<Systems::VelocitySystem> _velocitySystem;
  std::shared_ptr<Systems::MovementSystem> _movementSystem;
  std::shared_ptr<Systems::WeaponSystem> _weaponSystem;
  std::shared_ptr<Systems::RenderSystem> _renderSystem;
};