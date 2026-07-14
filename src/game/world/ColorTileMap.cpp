#include "world/ColorTileMap.h"

#include <SDL3_image/SDL_image.h>

#include <algorithm>
#include <limits>

#include "Logger.h"

namespace game::world {

namespace {

// ---------------------------------------------------------------------------
// Palette sampled directly from tilemap.png. If you repaint the source image
// with different colours, update these to match (or reclassification will
// just fall back to whichever entry happens to be closest).
// ---------------------------------------------------------------------------
constexpr SDL_Color kWaterColor = {95, 205, 228, 255};
constexpr SDL_Color kSandColor = {251, 238, 98, 255};
constexpr SDL_Color kGrassColor = {106, 190, 48, 255};
constexpr SDL_Color kForestColor = {0, 190, 25, 255};

int colorDistSq(Uint8 r, Uint8 g, Uint8 b, const SDL_Color& c) {
  const int dr = static_cast<int>(r) - c.r;
  const int dg = static_cast<int>(g) - c.g;
  const int db = static_cast<int>(b) - c.b;
  return dr * dr + dg * dg + db * db;
}

}  // namespace

bool ColorTileMap::loadFromImage(const std::string& imagePath, int cols, int rows) {
  SDL_Surface* surf = IMG_Load(imagePath.c_str());
  if (!surf) {
    core::Logger::log("ColorTileMap: failed to load '" + imagePath +
                      "': " + std::string(SDL_GetError()));
    return false;
  }

  cols_ = cols;
  rows_ = rows;
  kinds_.assign(static_cast<size_t>(cols_ * rows_), ColorTileKind::Grass);

  for (int row = 0; row < rows_; ++row) {
    for (int col = 0; col < cols_; ++col) {
      // Sample at the centre of the region this cell covers.
      const int px = static_cast<int>((col + 0.5f) / cols_ * surf->w);
      const int py = static_cast<int>((row + 0.5f) / rows_ * surf->h);

      Uint8 r = 0, g = 0, b = 0, a = 255;
      SDL_ReadSurfacePixel(surf, px, py, &r, &g, &b, &a);

      kinds_[static_cast<size_t>(row * cols_ + col)] = classify(r, g, b);
    }
  }

  SDL_DestroySurface(surf);
  return true;
}

ColorTileKind ColorTileMap::kindAt(int col, int row) const {
  if (col < 0 || col >= cols_ || row < 0 || row >= rows_) {
    return ColorTileKind::Water;  // treat out-of-bounds as water/solid
  }
  return kinds_[static_cast<size_t>(row * cols_ + col)];
}

bool ColorTileMap::isSolid(int col, int row) const {
  const ColorTileKind kind = kindAt(col, row);
  return kind == ColorTileKind::Water || kind == ColorTileKind::Forest;
}

void ColorTileMap::render(SDL_Renderer* renderer, float tileDst) const {
  if (!renderer || tileDst <= 0.0f) return;

  Uint8 prevR, prevG, prevB, prevA;
  SDL_GetRenderDrawColor(renderer, &prevR, &prevG, &prevB, &prevA);

  for (int row = 0; row < rows_; ++row) {
    for (int col = 0; col < cols_; ++col) {
      const SDL_Color c = colorFor(kindAt(col, row));
      SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);

      SDL_FRect dst{col * tileDst, row * tileDst, tileDst, tileDst};
      SDL_RenderFillRect(renderer, &dst);
    }
  }

  SDL_SetRenderDrawColor(renderer, prevR, prevG, prevB, prevA);
}

SDL_Color ColorTileMap::colorFor(ColorTileKind kind) {
  switch (kind) {
    case ColorTileKind::Water:
      return kWaterColor;
    case ColorTileKind::Sand:
      return kSandColor;
    case ColorTileKind::Grass:
      return kGrassColor;
    case ColorTileKind::Forest:
      return kForestColor;
  }
  return kWaterColor;
}

ColorTileKind ColorTileMap::classify(Uint8 r, Uint8 g, Uint8 b) {
  const SDL_Color palette[] = {kWaterColor, kSandColor, kGrassColor, kForestColor};
  const ColorTileKind kinds[] = {ColorTileKind::Water, ColorTileKind::Sand, ColorTileKind::Grass,
                                 ColorTileKind::Forest};

  int bestDist = std::numeric_limits<int>::max();
  ColorTileKind best = ColorTileKind::Grass;

  for (int i = 0; i < 4; ++i) {
    const int dist = colorDistSq(r, g, b, palette[i]);
    if (dist < bestDist) {
      bestDist = dist;
      best = kinds[i];
    }
  }
  return best;
}

}  // namespace game::world
