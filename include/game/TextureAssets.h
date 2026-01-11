#pragma once

#include <unordered_map>
#include <string>

// Texture ID map and file paths used by the game.

enum class TextureID
{
  Player,
  LaserBeam
};

inline std::unordered_map<TextureID, std::string> TextureAssets =
    {
        {TextureID::Player, "../images/spaceships/ship/purple.png"},
        {TextureID::LaserBeam, "../images/laserbeam.png"}};