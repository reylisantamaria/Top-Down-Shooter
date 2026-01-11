#pragma once

#include <SDL3/SDL.h>
#include <cmath>
#include <limits>

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

    // Subtraction
    constexpr Vec2 operator-(const Vec2 &other) const { return {x - other.x, y - other.y}; }
    constexpr Vec2 &operator-=(const Vec2 &other)
    {
      x -= other.x;
      y -= other.y;
      return *this;
    }

    // multiplication
    constexpr Vec2 operator*(const Vec2 &other) const { return {x * other.x, y * other.y}; }
    constexpr Vec2 &operator*=(const Vec2 &other)
    {
      x *= other.x;
      y *= other.y;
      return *this;
    }

    // division
    constexpr Vec2 operator/(const Vec2 &other) const { return {x / other.x, y / other.y}; }
    constexpr Vec2 &operator/=(const Vec2 &other)
    {
      x /= other.x;
      y /= other.y;
      return *this;
    }

    // Scalar multiplication
    constexpr Vec2 operator*(float scalar) const { return {x * scalar, y * scalar}; }
    constexpr Vec2 &operator*=(float scalar)
    {
      x *= scalar;
      y *= scalar;
      return *this;
    }

    // Scalar division
    constexpr Vec2 operator/(float scalar) const { return {x / scalar, y / scalar}; }
    constexpr Vec2 &operator/=(float scalar)
    {
      x /= scalar;
      y /= scalar;
      return *this;
    }

    // Normalize in place - returns true if normalized, false if magnitude is zero
    // does not modify the vector if the magnitude is zero
    constexpr bool normalize()
    {
      float magnitude = std::sqrt(x * x + y * y);
      if (magnitude <= std::numeric_limits<float>::epsilon())
        return false; // comparing floats leads to precision issues, so we use a small epsilon
      x /= magnitude;
      y /= magnitude;
      return true;
    }

    // Get normalized copy without modifying the original
    Vec2 normalized() const
    {
      Vec2 result = *this;
      result.normalize();
      return result;
    }
  };
}

// used to represent directions in 2D space
namespace Directions
{
  // since the window starts at (0,0) in the top left, we need to invert the y axis
  static constexpr Engine::Vec2 UP{0, -1};
  static constexpr Engine::Vec2 DOWN{0, 1};
  static constexpr Engine::Vec2 LEFT{-1, 0};
  static constexpr Engine::Vec2 RIGHT{1, 0};
}