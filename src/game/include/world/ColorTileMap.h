#pragma once

#include <SDL3/SDL.h>

#include <string>
#include <vector>

namespace game::world {

// ---------------------------------------------------------------------------
// ColorTileKind
//   The four "tiles" your source image currently encodes as flat colours.
//   These map 1:1 onto the layers described in the aseprite export
//   ("Water", "Ground", "Obj") plus the background.
// ---------------------------------------------------------------------------
enum class ColorTileKind {
  Water,   // background / lake colour  -> solid, deep water
  Sand,    // beach / shoreline colour  -> passable
  Grass,   // ground colour             -> passable
  Forest,  // "Obj" colour (trees etc.) -> solid
};

// ---------------------------------------------------------------------------
// ColorTileMap
//   A drop-in stand-in for TileMap + Tileset while you don't have real tile
//   art yet. Instead of indexing into a spritesheet, it loads a single
//   hand-painted PNG (tilemap.png), down-samples it onto a cols x rows grid,
//   and classifies each sampled pixel to the nearest of the four palette
//   colours above. render() then just fills a coloured rect per cell.
//
//   Once real tile art is ready, stop calling loadFromImage()/render() here
//   and use TileMap + Tileset instead — see World::MapSource.
// ---------------------------------------------------------------------------
class Camera;

class ColorTileMap {
 public:
  ColorTileMap() = default;

  /// Loads `imagePath`, resamples it onto a cols x rows grid and classifies
  /// each cell. Returns false if the image failed to load.
  bool loadFromImage(const std::string& imagePath, int cols, int rows);

  ColorTileKind kindAt(int col, int row) const;
  bool isSolid(int col, int row) const;

  int cols() const { return cols_; }
  int rows() const { return rows_; }

  /// Fills a coloured rect per cell, `tileDst` pixels square.
  void render(SDL_Renderer* renderer, const Camera& camera, float tileDst) const;

 private:
  int cols_ = 0;
  int rows_ = 0;
  std::vector<ColorTileKind> kinds_;

  static SDL_Color colorFor(ColorTileKind kind);
  static ColorTileKind classify(Uint8 r, Uint8 g, Uint8 b);
};

}  // namespace game::world
