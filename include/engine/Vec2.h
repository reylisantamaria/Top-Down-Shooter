#pragma once

#include <SDL3/SDL.h>
#include <cmath>

namespace Engine
{
  /**
   * Vec2 - A 2D vector {x, y}
   * used for position, velocity, direction, etc.
   */
  struct Vec2
  {
    float x = 0.0f;
    float y = 0.0f;

    // Constructors
    constexpr Vec2() = default;
    constexpr Vec2(float x, float y) : x(x), y(y) {}

    // Addition
    constexpr Vec2 operator+(const Vec2 &other) const { return {x + other.x, y + other.y}; }
    constexpr Vec2 &operator+=(const Vec2 &other)
    {
      x += other.x;
      y += other.y;
      return *this;
    }

    // Multiplication with another 2d vector
    constexpr Vec2 operator*(const Vec2 &other) const { return {x * other.x, y * other.y}; }
    constexpr Vec2 &operator*=(const Vec2 &other)
    {
      x *= other.x;
      y *= other.y;
      return *this;
    }

    // Multiplication with a scalar
    constexpr Vec2 operator*(float scalar) const { return {x * scalar, y * scalar}; }
    constexpr Vec2 &operator*=(float scalar)
    {
      x *= scalar;
      y *= scalar;
      return *this;
    }

    // normalize - returns true if the vector was normalized, false if zero-length
    constexpr bool normalize() 
    { 
      float len = this->length();
      if (len == 0.0f) return false;
      x /= len;
      y /= len;
      return true;
    }

    // length
    constexpr float length() const { return std::sqrt(x * x + y * y); }

  };
}

namespace Directions
{
  // since the window starts at (0,0) in the top left, we need to invert the y axis
  static constexpr Engine::Vec2 UP{0, -1};
  static constexpr Engine::Vec2 DOWN{0, 1};
  static constexpr Engine::Vec2 LEFT{-1, 0};
  static constexpr Engine::Vec2 RIGHT{1, 0};
}