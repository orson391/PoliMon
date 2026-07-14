#include "world/TileMap.h"

#include <algorithm>

namespace game::world {

void TileMap::init(int cols, int rows, std::vector<int> tileIds) {
  cols_  = cols;
  rows_  = rows;
  tiles_ = std::move(tileIds);

  // Pad or truncate to exactly cols * rows entries
  tiles_.resize(static_cast<size_t>(cols_ * rows_), -1);
}

int TileMap::tileAt(int col, int row) const {
  if (col < 0 || col >= cols_ || row < 0 || row >= rows_) return -1;
  return tiles_[static_cast<size_t>(row * cols_ + col)];
}

void TileMap::render(SDL_Renderer* renderer, const Tileset& tileset,
                     float scale) const {
  if (!renderer || !tileset.texture()) return;

  const float dstW = static_cast<float>(tileset.tileWidth())  * scale;
  const float dstH = static_cast<float>(tileset.tileHeight()) * scale;

  for (int row = 0; row < rows_; ++row) {
    for (int col = 0; col < cols_; ++col) {
      const int id = tileAt(col, row);
      if (id < 0) continue;  // empty cell

      SDL_FRect src = tileset.srcRect(id);
      SDL_FRect dst{col * dstW, row * dstH, dstW, dstH};

      SDL_RenderTexture(renderer, tileset.texture(), &src, &dst);
    }
  }
}

}  // namespace game::world
