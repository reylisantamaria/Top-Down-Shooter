#include "GameObject.h"

class Laser : public GameObject {
  public:
    Laser(SDL_Renderer* renderer, const char* path, float x, float y, float w, float h)
      : GameObject(renderer, path, x, y, w, h) {}
};