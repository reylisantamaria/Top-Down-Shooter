#pragma once

#include <unordered_map>
#include <string>

// This file focuses on all texture asset IDs and file paths for the game.

enum class TextureID
{
  Player,
  Background
};

inline std::unordered_map<TextureID, std::string> TextureAssets = {
    {TextureID::Player, "../images/spaceships/ship/purple.png"},
    {TextureID::Background, "../images/bg/background.png"}};