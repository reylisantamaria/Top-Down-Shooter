#include "GameObject.h"

class Meteor : public GameObject {
  public:
    Meteor(SDL_Renderer* renderer, const char* path, float x, float y, float w, float h)
      : GameObject(renderer, path, x, y, w, h) {}
};