#include "GameObject.h"

class Player : public GameObject {
  public:
    Player(SDL_Renderer* renderer, const char* path, float x, float y, float w, float h)
      : GameObject(renderer, path, x, y, w, h), speed_(300.0f) {}

    Player(SDL_Renderer* renderer, const char* path, float w, float h) : GameObject(renderer, path, w, h), speed_(300.0f) {}

    // Update the player
    void Update(const bool* keystate, float dt) {
      HandleInput(keystate);
      GameObject::rect_.x += GameObject::direction_.x * speed_ * dt;
      GameObject::rect_.y += GameObject::direction_.y * speed_ * dt;
    }

    // Handle input
    void HandleInput(const bool* keystate) {
      // Get the direction of the player)
      // eg. if D is pressed and A is not = 1 - 0 = 1 or if A is pressed and D is not = 0 - 1 = -1
      GameObject::direction_.x = (keystate[SDL_SCANCODE_D] - keystate[SDL_SCANCODE_A]) * speed_;
      GameObject::direction_.y = (keystate[SDL_SCANCODE_S] - keystate[SDL_SCANCODE_W]) * speed_;

      // normalize the direction so that the diagonal movement is not faster than the cardinal directions
      if (GameObject::direction_.x != 0 || GameObject::direction_.y != 0) {
        GameObject::direction_.normalize();
      }
    }
  private:
    float speed_;
};