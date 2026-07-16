#include "world/Tileset.h"

#include "Logger.h"

namespace game::world {

bool Tileset::load(SDL_Renderer* renderer, const TilesetDef& def) {
  def_ = def;

  SDL_Texture* tex = IMG_LoadTexture(renderer, def_.texturePath.c_str());
  if (!tex) {
    core::Logger::log("Tileset: failed to load '" + def_.texturePath +
                      "': " + std::string(SDL_GetError()));
    return false;
  }

  texture_.reset(tex);

  // Compute grid dimensions from the texture size
  float w = 0.0f, h = 0.0f;
  SDL_GetTextureSize(tex, &w, &h);

  const int texW = static_cast<int>(w);
  const int texH = static_cast<int>(h);

  const int cellW = def_.tileWidth  + def_.spacing;
  const int cellH = def_.tileHeight + def_.spacing;

  cols_ = (texW - 2 * def_.margin + def_.spacing) / cellW;
  rows_ = (texH - 2 * def_.margin + def_.spacing) / cellH;

  if (cols_ <= 0) cols_ = 1;
  if (rows_ <= 0) rows_ = 1;

  return true;
}

SDL_FRect Tileset::srcRect(int tileId) const {
  if (cols_ <= 0) return {0, 0, static_cast<float>(def_.tileWidth), static_cast<float>(def_.tileHeight)};

  const int col = tileId % cols_;
  const int row = tileId / cols_;

  const float x = static_cast<float>(def_.margin + col * (def_.tileWidth  + def_.spacing));
  const float y = static_cast<float>(def_.margin + row * (def_.tileHeight + def_.spacing));

  return {x, y, static_cast<float>(def_.tileWidth), static_cast<float>(def_.tileHeight)};
}

}  // namespace game::world
