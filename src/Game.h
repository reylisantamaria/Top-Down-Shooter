#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "Entities/Player.h"

#include <memory>

class Game {
  public:
    Game();
    bool init();
    void run();
  private:
    SDL_Window* window_;
    SDL_Renderer* renderer_;
    bool running_;
    std::shared_ptr<Player> player_;
};