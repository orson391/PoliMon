#include "world/CollisionLayer.h"

#include <algorithm>
#include <cmath>

namespace game::world {

void CollisionLayer::init(int cols, int rows, std::vector<bool> solid) {
  cols_  = cols;
  rows_  = rows;
  solid_ = std::move(solid);
  solid_.resize(static_cast<size_t>(cols_ * rows_), false);
}

bool CollisionLayer::isSolid(int col, int row) const {
  if (col < 0 || col >= cols_ || row < 0 || row >= rows_) return true;  // out of bounds = solid
  return solid_[static_cast<size_t>(row * cols_ + col)];
}

bool CollisionLayer::overlaps(SDL_FRect worldRect,
                               float tileDstW, float tileDstH) const {
  if (tileDstW <= 0.0f || tileDstH <= 0.0f) return false;

  // Tile range covered by the rect (inclusive)
  const int colMin = static_cast<int>(std::floor(worldRect.x / tileDstW));
  const int colMax = static_cast<int>(std::floor((worldRect.x + worldRect.w - 1.0f) / tileDstW));
  const int rowMin = static_cast<int>(std::floor(worldRect.y / tileDstH));
  const int rowMax = static_cast<int>(std::floor((worldRect.y + worldRect.h - 1.0f) / tileDstH));

  for (int row = rowMin; row <= rowMax; ++row) {
    for (int col = colMin; col <= colMax; ++col) {
      if (isSolid(col, row)) return true;
    }
  }
  return false;
}

}  // namespace game::world
