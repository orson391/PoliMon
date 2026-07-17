#pragma once

#include <memory>
#include <string>

#include "IRenderer.h"

namespace game::world {

// ---------------------------------------------------------------------------
// TilesetDef
//   Describes how to interpret a spritesheet as a uniform grid of tiles.
//   tileWidth / tileHeight : size of one tile in pixels (source image space)
//   spacing                : transparent gap between tiles (pixels)
//   margin                 : border around the whole sheet (pixels)
// ---------------------------------------------------------------------------
struct TilesetDef {
  std::string texturePath;
  int tileWidth  = 64;
  int tileHeight = 64;
  int spacing    = 0;
  int margin     = 0;
};

// ---------------------------------------------------------------------------
// Tileset
//   Owns a graphics texture for one spritesheet and can return the source rect
//   for any tile ID (0-based, row-major order).
// ---------------------------------------------------------------------------
class Tileset {
 public:
  Tileset() = default;

  /// Load the texture from the definition. Returns false on failure.
  bool load(::graphics::IRenderer& renderer, const TilesetDef& def);

  /// Source rect in the sheet for a given tile id.
  ::graphics::Rect srcRect(int tileId) const;

  const std::shared_ptr<::graphics::Texture>& texture() const { return texture_; }
  int          tileWidth()  const { return def_.tileWidth; }
  int          tileHeight() const { return def_.tileHeight; }
  int          cols()       const { return cols_; }
  int          rows()       const { return rows_; }

 private:
  std::shared_ptr<::graphics::Texture> texture_;
  TilesetDef def_;
  int cols_ = 0;
  int rows_ = 0;
};

}  // namespace game::world
