#include "world/TileMap.h"
#include "world/Camera.h"
#include <cmath>

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
                     const Camera& camera, float scale) const {
  if (!renderer || !tileset.texture()) return;

  const float dstW = static_cast<float>(tileset.tileWidth())  * scale;
  const float dstH = static_cast<float>(tileset.tileHeight()) * scale;

  const float zoom = camera.zoom();

  // Frustum culling: calculate only the columns and rows that intersect the zoomed camera viewport
  int startCol = std::max(0, static_cast<int>(std::floor(camera.x() / dstW)));
  int endCol = std::min(cols_ - 1, static_cast<int>(std::floor((camera.x() + camera.viewportWidth() / zoom) / dstW)));
  int startRow = std::max(0, static_cast<int>(std::floor(camera.y() / dstH)));
  int endRow = std::min(rows_ - 1, static_cast<int>(std::floor((camera.y() + camera.viewportHeight() / zoom) / dstH)));

  for (int row = startRow; row <= endRow; ++row) {
    for (int col = startCol; col <= endCol; ++col) {
      const int id = tileAt(col, row);
      if (id < 0) continue;  // empty cell

      SDL_FRect src = tileset.srcRect(id);
      SDL_FRect dst{(col * dstW - camera.x()) * zoom, (row * dstH - camera.y()) * zoom, dstW * zoom, dstH * zoom};

      SDL_RenderTexture(renderer, tileset.texture(), &src, &dst);
    }
  }
}

}  // namespace game::world
