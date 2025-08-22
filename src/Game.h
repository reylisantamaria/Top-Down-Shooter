#include <SDL3/SDL.h>

class Game {
  public:
    Game();
    void init();
    void run();
  private:
    SDL_Window* window_;
    SDL_Renderer* renderer_;
    bool running_;
};