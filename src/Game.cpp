#include "Game.h"
#include <iostream>

constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 720;
constexpr int WINDOW_FLAGS = SDL_WINDOW_RESIZABLE;

Game::Game() {
  if (!init()) {
    std::cerr << "Failed to initialize game" << std::endl;
    exit(1);
  }
}

bool Game::init() {
  // Initialize SDL
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
    return false;
  }

  // Create window
  window_ = SDL_CreateWindow("Top-Down Shooter", WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_FLAGS);
  if (window_ == nullptr) {
    std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    SDL_Quit();
    return false;
  }

  // Create renderer
  renderer_ = SDL_CreateRenderer(window_, nullptr);
  if (renderer_ == nullptr) {
    std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    SDL_DestroyWindow(window_);
    SDL_Quit();
    return false;
  }
  running_ = true;

  // Create player
  player_ = std::make_shared<Player>(renderer_, "../images/spaceships/ship/purple.png", 100.0f, 100.0f);
  player_->CenterToScreen(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

  return true;
}

void Game::run() {

  Uint64 lastTime = SDL_GetPerformanceCounter();
  float deltaTime = 0.0f;

  // Main loop
  while (running_) {
    // Get the current time
    Uint64 currentTime = SDL_GetPerformanceCounter();
    // Calculate the delta time
    deltaTime = (currentTime - lastTime) / (float)SDL_GetPerformanceFrequency();
    // Update the last time
    lastTime = currentTime;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        running_ = false;
      }
    }
    const bool* keystate = SDL_GetKeyboardState(NULL);
    player_->Update(keystate, deltaTime);

    // Draw background
    SDL_SetRenderDrawColor(renderer_, 25, 25, 25, 255);

    // Clear screen
    SDL_RenderClear(renderer_);

    // Draw player
    player_->draw(renderer_);

    // Update screen
    SDL_RenderPresent(renderer_);
  }

  // Clean up
  SDL_DestroyRenderer(renderer_);
  SDL_DestroyWindow(window_);

  SDL_Quit();
}