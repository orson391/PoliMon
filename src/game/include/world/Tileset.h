#pragma once

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <memory>
#include <string>

namespace game::world {

// ---------------------------------------------------------------------------
// SDLTextureDeleter
// ---------------------------------------------------------------------------
struct SDLTextureDeleter {
  void operator()(SDL_Texture* tex) const {
    if (tex) SDL_DestroyTexture(tex);
  }
};

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
//   Owns an SDL_Texture for one spritesheet and can return the source rect
//   for any tile ID (0-based, row-major order).
// ---------------------------------------------------------------------------
class Tileset {
 public:
  Tileset() = default;

  /// Load the texture from the definition. Returns false on failure.
  bool load(SDL_Renderer* renderer, const TilesetDef& def);

  /// Source rect in the sheet for a given tile id.
  SDL_FRect srcRect(int tileId) const;

  SDL_Texture* texture()    const { return texture_.get(); }
  int          tileWidth()  const { return def_.tileWidth; }
  int          tileHeight() const { return def_.tileHeight; }
  int          cols()       const { return cols_; }
  int          rows()       const { return rows_; }

 private:
  std::unique_ptr<SDL_Texture, SDLTextureDeleter> texture_;
  TilesetDef def_;
  int cols_ = 0;
  int rows_ = 0;
};

}  // namespace game::world
