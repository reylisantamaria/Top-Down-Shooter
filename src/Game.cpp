#include "Game.h"
#include "game/TextureAssets.h"
#include "game/EntityCreator.h"
#include "engine/TextureManager.h"
#include <iostream>

// Window configuration
namespace Config
{
  constexpr int WINDOW_WIDTH = 1280;
  constexpr int WINDOW_HEIGHT = 720;
  constexpr Uint32 WINDOW_FLAGS = SDL_WINDOW_RESIZABLE;
  constexpr const char *WINDOW_TITLE = "Top-Down Shooter";
}

bool Game::Init()
{
  if (!InitSDL())
  {
    Cleanup();
    return false;
  }
  if (!InitECS())
  {
    Cleanup();
    return false;
  }
  if (!LoadAssets())
  {
    Cleanup();
    return false;
  }
  return true;
}

Game::~Game()
{
  Cleanup();
}

bool Game::InitSDL()
{
  if (!SDL_Init(SDL_INIT_VIDEO))
  {
    std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
    return false;
  }

  _window = SDL_CreateWindow(
      Config::WINDOW_TITLE,
      Config::WINDOW_WIDTH,
      Config::WINDOW_HEIGHT,
      Config::WINDOW_FLAGS);

  if (!_window)
  {
    std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
    SDL_Quit();
    return false;
  }

  _renderer = SDL_CreateRenderer(_window, nullptr);
  if (!_renderer)
  {
    std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
    SDL_DestroyWindow(_window);
    _window = nullptr;
    SDL_Quit();
    return false;
  }

  _running = true;
  return true;
}

bool Game::InitECS()
{
  auto &coordinator = Engine::Coordinator::GetInstance();

  // Register input system
  _playerInputSystem = coordinator.RegisterSystem<Systems::PlayerInputSystem,
                                                  Components::Player,
                                                  Components::MoveIntent,
                                                  Components::AimIntent,
                                                  Components::FireIntent>();

  // Register weapon system
  _weaponSystem = coordinator.RegisterSystem<Systems::WeaponSystem,
                                             Components::Weapon,
                                             Components::Cooldown,
                                             Components::OwnedBy,
                                             Components::WeaponStats>();

  // Register timer systems
  _cooldownSystem = coordinator.RegisterSystem<Systems::CooldownSystem,
                                               Components::Cooldown>();

  _lifetimeSystem = coordinator.RegisterSystem<Systems::LifetimeSystem,
                                               Components::Lifetime>();

  // Register gameplay systems
  _aimSystem = coordinator.RegisterSystem<Systems::AimSystem,
                                          Components::Transform,
                                          Components::AimIntent>();

  // Update velocity's values based on the entity's move intent
  _velocitySystem = coordinator.RegisterSystem<Systems::VelocitySystem,
                                                 Components::MoveIntent,
                                                 Components::Velocity,
                                                 Components::Speed>();

  // Move the entity's transform based on its velocity
  _movementSystem = coordinator.RegisterSystem<Systems::MovementSystem,
                                               Components::Transform,
                                               Components::Velocity>();

  // Render the entity's sprite
  _renderSystem = coordinator.RegisterSystem<Systems::RenderSystem,
                                             Components::Transform,
                                             Components::Sprite>();

  // Initialize render system with renderer and texture manager
  _renderSystem->Init(_renderer, &Engine::TextureManager::GetInstance());

  // Create player
  Engine::Entity player = EntityCreator::CreatePlayer({.position = {Config::WINDOW_WIDTH / 2.0f, Config::WINDOW_HEIGHT / 2.0f}});

  // Create weapon for player
  EntityCreator::CreateLaserWeapon(player);

  return true;
}

bool Game::LoadAssets()
{
  auto &texManager = Engine::TextureManager::GetInstance();
  texManager.Init(_renderer);
  texManager.LoadAllTextures();

  return true;
}

void Game::Run()
{
  float deltaTime = 0.0f;

  while (_running)
  {
    Uint64 frameStart = SDL_GetTicks();

    HandleEvents();
    Update(deltaTime);
    Render();

    Uint64 frameDuration = SDL_GetTicks() - frameStart;
    deltaTime = frameDuration / 1000.0f;
  }
}

void Game::HandleEvents()
{
  SDL_Event event;
  while (SDL_PollEvent(&event))
  {
    if (event.type == SDL_EVENT_QUIT)
    {
      _running = false;
    }
  }
}

void Game::Update(float deltaTime)
{
  // 1. Input: Read player input
  _playerInputSystem->Update();

  // 2. Decision: Calculate aim directions
  _aimSystem->Update();

  // 3. Action: Fire weapons
  _weaponSystem->Update();

  // 4. Movement: Convert intents to velocity, then move
  _velocitySystem->Update();
  _movementSystem->Update(deltaTime);

  // 5. Timers: Update cooldowns and lifetimes
  _cooldownSystem->Update(deltaTime);
  _lifetimeSystem->Update(deltaTime);

  auto &coordinator = Engine::Coordinator::GetInstance();

  auto entityCount = coordinator.GetEntityCount();
  static auto prevEntityCount = entityCount;
  if (entityCount != prevEntityCount)
  {
    std::println("Entity count: {}", entityCount);
    prevEntityCount = entityCount;
  }
}

void Game::Render() const
{
  // Clear screen with dark gray color
  SDL_SetRenderDrawColor(_renderer, 25, 25, 25, 255);
  SDL_RenderClear(_renderer);

  // Render all entities with the render system
  _renderSystem->Update();

  SDL_RenderPresent(_renderer);
}

void Game::Cleanup()
{
  if (_renderer)
  {
    SDL_DestroyRenderer(_renderer);
    _renderer = nullptr;
  }
  if (_window)
  {
    SDL_DestroyWindow(_window);
    _window = nullptr;
  }

  std::println("Cleaning up game...");
  SDL_Quit();
}