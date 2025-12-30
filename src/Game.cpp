#include "Game.h"
#include "game/TextureAssets.h"
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
  auto& coordinator = Engine::Coordinator::GetInstance();
  std::println("Entity count: {}", coordinator.GetEntityCount());
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

  // Register and configure render system
  // Components are automatically registered when used
  _renderSystem = coordinator.RegisterSystem<Systems::RenderSystem,
                                             Components::Transform,
                                             Components::Sprite>();

  // Register and configure input system (player only)
  _inputSystem = coordinator.RegisterSystem<Systems::InputSystem,
                                            Components::PlayerTag,
                                            Components::Velocity,
                                            Components::Direction>();

  // Register and configure movement system
  _movementSystem = coordinator.RegisterSystem<Systems::MovementSystem,
                                               Components::Transform,
                                               Components::Velocity>();

  // Register and configure aim system
  _aimSystem = coordinator.RegisterSystem<Systems::AimSystem,
                                          Components::Transform>();

  // Initialize render system with renderer and texture manager
  _renderSystem->Init(_renderer, &Engine::TextureManager::GetInstance());

  _playerEntity = coordinator.CreateEntity();

  coordinator.AddComponent<Components::Transform>(_playerEntity, {.position = {400.0f, 300.0f}});

  coordinator.AddComponent<Components::Sprite>(_playerEntity, {.textureId = TextureID::Player,
                                                               .srcRect = {0.0f, 0.0f, 64.0f, 64.0f}, // Adjust to your texture size
                                                               .scaleMode = SDL_SCALEMODE_NEAREST,    // Pixel art style
                                                               .flipMode = SDL_FLIP_NONE});

  coordinator.AddComponent<Components::Velocity>(_playerEntity, {.speed = 300.0f});

  coordinator.AddComponent<Components::Direction>(_playerEntity, {Directions::RIGHT});

  coordinator.AddComponent<Components::PlayerTag>(_playerEntity, {});

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
  _inputSystem->Update();
  _movementSystem->Update(deltaTime);
  _aimSystem->Update();
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