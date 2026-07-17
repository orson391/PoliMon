#pragma once

#include <vector>

#include "world/Tileset.h"

namespace game::world {

// ---------------------------------------------------------------------------
// TileMap
//   Stores a 2-D grid of tile IDs (row-major). Each ID indexes into a Tileset.
//   A value of -1 means "empty / transparent".
//
//   render() draws each non-empty tile scaled to (tileWidth * scale) x
//   (tileHeight * scale) destination pixels.
// ---------------------------------------------------------------------------
class Camera;

class TileMap {
 public:
  TileMap() = default;

  /// Populate with hard-coded or procedurally generated data.
  void init(int cols, int rows, std::vector<int> tileIds);

  void render(::graphics::IRenderer& renderer, const Tileset& tileset,
              const Camera& camera, float scale = 1.0f) const;

  int  cols()              const { return cols_; }
  int  rows()              const { return rows_; }
  int  tileAt(int col, int row) const;

 private:
  int              cols_ = 0;
  int              rows_ = 0;
  std::vector<int> tiles_;   // row-major; -1 = empty
};

}  // namespace game::world
