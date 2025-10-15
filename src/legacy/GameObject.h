#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <cmath>
#include <iostream>

struct Vector2 {
  float x;
  float y;

  void normalize() {
    float length = sqrt(x * x + y * y);
    if (length > 0) {
      x /= length;
      y /= length;
    }
  }
};

class GameObject {
  public:
    // Constructor
    GameObject(SDL_Renderer* renderer, const char* path, float x, float y, float w, float h) 
    : rect_{x, y, w, h}, texture_{nullptr}, direction_{0, 0} {

      SDL_Surface* surface = IMG_Load(path); // load the image from the path
      texture_ = SDL_CreateTextureFromSurface(renderer, surface);   // create texture from surface
      SDL_SetTextureScaleMode(texture_, SDL_SCALEMODE_NEAREST);    // pixel perfect scaling
      if (texture_ == nullptr) {
        std::cerr << "Failed to load texture: " << path << std::endl;
        exit(1);
      }
      SDL_DestroySurface(surface);

    }
    // Constructor with texture, size, but no position
    GameObject(SDL_Renderer* renderer, const char* path, float w, float h) : rect_{0, 0, w, h}, texture_{nullptr}, direction_{0, 0} {

      SDL_Surface* surface = IMG_Load(path); // load the image from the path
      texture_ = SDL_CreateTextureFromSurface(renderer, surface);   // create texture from surface
      SDL_SetTextureScaleMode(texture_, SDL_SCALEMODE_NEAREST);    // pixel perfect scaling
      if (texture_ == nullptr) {
        std::cerr << "Failed to load texture: " << path << std::endl;
        exit(1);
      }
      SDL_DestroySurface(surface);

    }
    // Destructor
    ~GameObject() {
      if (texture_ != nullptr) {
        SDL_DestroyTexture(texture_);
      }
    }
    // Draw the game object to the screen
    void draw(SDL_Renderer* renderer) {
      SDL_RenderTexture(renderer, texture_, nullptr, &rect_);
    }

    void CenterToScreen(float centerX, float centerY) {
      rect_.x = centerX - rect_.w / 2;
      rect_.y = centerY - rect_.h / 2;
    }

  protected:
    SDL_FRect rect_;  // Floating point rectangle
    SDL_Texture* texture_;  // Texture
    Vector2 direction_;
  };