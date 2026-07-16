#pragma once

#include <SDL3/SDL.h>

#include <vector>

namespace game::world {

// ---------------------------------------------------------------------------
// CollisionLayer
//   Parallel boolean grid to a TileMap — each cell is either solid (true)
//   or passable (false).
//
//   overlaps() tests whether a world-space AABB intersects any solid cell,
//   allowing Player::update() to block movement before it happens.
// ---------------------------------------------------------------------------
class CollisionLayer {
 public:
  CollisionLayer() = default;

  /// Build from a flat bool vector (row-major). Length must be cols * rows.
  void init(int cols, int rows, std::vector<bool> solid);

  /// Returns true if (col, row) is a solid tile.
  bool isSolid(int col, int row) const;

  /// Returns true if a world-space rect overlaps any solid cell.
  /// tileW / tileH are destination pixel sizes (tileWidth * scale).
  bool overlaps(SDL_FRect worldRect, float tileDstW, float tileDstH) const;

  int cols() const { return cols_; }
  int rows() const { return rows_; }

 private:
  int               cols_ = 0;
  int               rows_ = 0;
  std::vector<bool> solid_;
};

}  // namespace game::world
