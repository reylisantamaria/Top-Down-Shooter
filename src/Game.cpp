#include "Game.h"
#include <iostream>


constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 720;
constexpr int WINDOW_FLAGS = SDL_WINDOW_RESIZABLE;

Game::Game() {
  init();
}

void Game::init() {
  // Initialize SDL
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
    return;
  }

  // Create window
  window_ = SDL_CreateWindow("Top-Down Shooter", WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_FLAGS);
  if (window_ == nullptr) {
    std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    return;
  }

  // Create renderer
  renderer_ = SDL_CreateRenderer(window_, nullptr);
  if (renderer_ == nullptr) {
    std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    return;
  }
  running_ = true;
}

void Game::run() {

  // Main loop
  while (running_) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        running_ = false;
      }
    }

    // Draw background
    SDL_SetRenderDrawColor(renderer_, 25, 25, 25, 255);

    // Clear screen
    SDL_RenderClear(renderer_);

    // Update screen
    SDL_RenderPresent(renderer_);
  }

  // Clean up
  SDL_DestroyRenderer(renderer_);
  SDL_DestroyWindow(window_);

  SDL_Quit();
}