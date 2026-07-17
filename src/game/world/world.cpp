#include "world/World.h"

#include <algorithm>

#include "ResourcePaths.h"
#include "Logger.h"
#include "entities/Player.h"

// ---------------------------------------------------------------------------
// Map constants — Tileset mode
//   The demo map is MAP_COLS x MAP_ROWS tiles.
//   Each tile is TILE_SRC x TILE_SRC source pixels, drawn at
//   TILE_SRC * TILE_SCALE destination pixels.
// ---------------------------------------------------------------------------
static constexpr int MAP_COLS = 64;
static constexpr int MAP_ROWS = 64;
static constexpr int TILE_SRC = 64;        // source tile size in all tilesets (px)
static constexpr float TILE_SCALE = 1.0f;  // destination = TILE_SRC * TILE_SCALE

// ---------------------------------------------------------------------------
// Map constants — ColorImage mode
//   tilemap.png is resampled onto a COLOR_GRID_COLS x COLOR_GRID_ROWS grid;
//   each cell is drawn COLOR_TILE_DST pixels square.
// ---------------------------------------------------------------------------
static constexpr int COLOR_GRID_COLS = 64;
static constexpr int COLOR_GRID_ROWS = 64;
static constexpr float COLOR_TILE_DST = 24.0f;

namespace game::world {

// ===========================================================================
// load
// ===========================================================================
bool World::load(SDL_Renderer* renderer, MapSource source) {
  source_ = source;
  bool ok = true;

  if (source_ == MapSource::ColorImage) {
    ok = loadColorMap(renderer);
    buildCollisionLayerFromColorMap();
  } else {
    ok = loadTilesetAssets(renderer);
    buildGroundMap();
    buildWaterMap();
    buildPropsMap();
    buildCollisionLayerFromTileset();
  }

  buildObjectLayer();

  if (source_ == MapSource::ColorImage) {
    camera_.setMapBounds(COLOR_GRID_COLS * COLOR_TILE_DST, COLOR_GRID_ROWS * COLOR_TILE_DST);
  } else {
    camera_.setMapBounds(MAP_COLS * TILE_SRC * TILE_SCALE, MAP_ROWS * TILE_SRC * TILE_SCALE);
  }

  return ok;
}

// ===========================================================================
// loadColorMap
//   ColorImage mode: resample tilemap.png onto a grid and classify each
//   cell by nearest palette colour. No tileset texture is needed.
// ===========================================================================
bool World::loadColorMap(SDL_Renderer* /*renderer*/) {
  if (!colorMap_.loadFromImage(core::ResourcePaths::map("tilemap.png").string(),
                               COLOR_GRID_COLS, COLOR_GRID_ROWS)) {
    core::Logger::log("World: failed to load colour tilemap.");
    return false;
  }
  return true;
}

// ===========================================================================
// loadTilesetAssets
//   Tileset mode: load the three real spritesheets.
// ===========================================================================
bool World::loadTilesetAssets(SDL_Renderer* renderer) {
  bool ok = true;

  // ---- Ground Tiles (2048x2048 — 32 cols x 32 rows of 64px tiles) --------
  {
    TilesetDef def;
    def.texturePath = core::ResourcePaths::texture("Ground Tiles.png").string();
    def.tileWidth = TILE_SRC;
    def.tileHeight = TILE_SRC;
    def.spacing = 0;
    def.margin = 0;
    if (!groundTileset_.load(renderer, def)) {
      core::Logger::log("World: failed to load ground tileset.");
      ok = false;
    }
  }

  // ---- Water Tiles (1024x1024 — 16 cols x 16 rows of 64px tiles) ---------
  {
    TilesetDef def;
    def.texturePath = core::ResourcePaths::texture("Water Tiles.png").string();
    def.tileWidth = TILE_SRC;
    def.tileHeight = TILE_SRC;
    def.spacing = 0;
    def.margin = 0;
    if (!waterTileset_.load(renderer, def)) {
      core::Logger::log("World: failed to load water tileset.");
      ok = false;
    }
  }

  // ---- Props (2048x2048 — 32 cols x 32 rows of 64px tiles) ---------------
  {
    TilesetDef def;
    def.texturePath = core::ResourcePaths::texture("Props.png").string();
    def.tileWidth = TILE_SRC;
    def.tileHeight = TILE_SRC;
    def.spacing = 0;
    def.margin = 0;
    if (!propsTileset_.load(renderer, def)) {
      core::Logger::log("World: failed to load props tileset.");
      ok = false;
    }
  }

  return ok;
}

// ===========================================================================
// update / render
// ===========================================================================
void World::update(float dt, float viewportWidth, float viewportHeight) {
  entityLayer_.update(dt);

  if (player_) {
    const float dx = player_->desiredMoveX(dt);
    const float dy = player_->desiredMoveY(dt);
    tryMove(*player_, dx, dy);
  }

  camera_.setViewport(viewportWidth, viewportHeight);
  if (player_) {
    camera_.follow(player_->x(), player_->y(), player_->width(), player_->height());
  }
}

bool World::tryMove(entities::Player& player, float dx, float dy) {
  if (dx == 0.0f && dy == 0.0f) {
    return true;
  }

  const float startX = player.x();
  const float startY = player.y();
  const float w = player.width();
  const float h = player.height();

  float nextX = startX;
  float nextY = startY;

  const auto moveAxis = [&](float delta, bool horizontal) {
    if (delta == 0.0f) return true;
    const float candidateX = horizontal ? nextX + delta : nextX;
    const float candidateY = horizontal ? nextY : nextY + delta;
    if (canOccupy(candidateX, candidateY, w, h)) {
      if (horizontal) {
        nextX = candidateX;
      } else {
        nextY = candidateY;
      }
      return true;
    }
    return false;
  };

  const bool movedX = moveAxis(dx, true);
  const bool movedY = moveAxis(dy, false);

  const float maxX = std::max(0.0f, mapWidth() - w);
  const float maxY = std::max(0.0f, mapHeight() - h);

  nextX = std::clamp(nextX, 0.0f, maxX);
  nextY = std::clamp(nextY, 0.0f, maxY);

  player.commitMove(nextX - startX, nextY - startY);
  return movedX && movedY;
}

void World::render(SDL_Renderer* renderer) {
  if (!renderer) return;

  if (source_ == MapSource::ColorImage) {
    colorMap_.render(renderer, camera_, COLOR_TILE_DST);
  } else {
    // Render order: ground -> water -> props (back to front)
    groundMap_.render(renderer, groundTileset_, camera_, TILE_SCALE);
    waterMap_.render(renderer, waterTileset_, camera_, TILE_SCALE);
    propsMap_.render(renderer, propsTileset_, camera_, TILE_SCALE);
  }

  entityLayer_.render(renderer, camera_);
}

// ===========================================================================
// tileDstSize
// ===========================================================================
float World::tileDstSize() const {
  return source_ == MapSource::ColorImage ? COLOR_TILE_DST : TILE_SRC * TILE_SCALE;
}

// ===========================================================================
// buildGroundMap
//   20x15 hard-coded demo map.
//   Ground tileset: 2048x2048 = 32 cols x 32 rows of 64px tiles.
//
//   Tile IDs used (row-major, 0-based):
//     0  = plain grass
//     1  = grass variant
//     32 = dirt path (row 1 of sheet, col 0)
// ===========================================================================
void World::buildGroundMap() {
  const int G = 0;   // plain grass
  const int g = 1;   // grass variant
  const int D = 32;  // dirt path

  // clang-format off
  std::vector<int> tiles = {
    // row 0
    G,G,G,G,G,G,G,G,G,G,G,G,G,G,G,G,G,G,G,G,
    // row 1
    G,g,g,G,G,G,G,G,G,G,G,G,G,G,G,G,g,g,G,G,
    // row 2
    G,g,G,G,G,G,G,G,G,G,G,G,G,G,G,G,G,g,G,G,
    // row 3
    G,G,G,G,G,G,G,G,G,G,D,G,G,G,G,G,G,G,G,G,
    // row 4
    G,G,G,G,G,G,G,G,G,G,D,G,G,G,G,G,G,G,G,G,
    // row 5
    G,G,G,G,G,G,G,G,G,G,D,G,G,G,G,G,G,G,G,G,
    // row 6
    G,G,G,G,G,G,G,G,G,G,D,G,G,G,G,G,G,G,G,G,
    // row 7  — horizontal dirt road
    G,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,G,
    // row 8
    G,G,G,G,G,G,G,G,G,G,D,G,G,G,G,G,G,G,G,G,
    // row 9
    G,G,G,G,G,G,G,G,G,G,D,G,G,G,G,G,G,G,G,G,
    // row 10
    G,G,G,G,G,G,G,G,G,G,D,G,G,G,G,G,G,G,G,G,
    // row 11
    G,G,G,G,G,G,G,G,G,G,D,G,G,G,G,G,G,G,G,G,
    // row 12
    G,g,G,G,G,G,G,G,G,G,D,G,G,G,G,G,G,g,G,G,
    // row 13
    G,g,g,G,G,G,G,G,G,G,D,G,G,G,G,G,g,g,G,G,
    // row 14
    G,G,G,G,G,G,G,G,G,G,D,G,G,G,G,G,G,G,G,G,
  };
  // clang-format on

  groundMap_.init(MAP_COLS, MAP_ROWS, std::move(tiles));
}

// ===========================================================================
// buildWaterMap
//   Water tileset: 1024x1024 = 16 cols x 16 rows of 64px tiles.
//   Pond at cols 2..6, rows 2..5. All other cells = -1 (transparent).
//   Tile 0 = open water centre.
// ===========================================================================
void World::buildWaterMap() {
  std::vector<int> tiles(static_cast<size_t>(MAP_COLS * MAP_ROWS), -1);

  for (int row = 2; row <= 5; ++row) {
    for (int col = 2; col <= 6; ++col) {
      tiles[static_cast<size_t>(row * MAP_COLS + col)] = 0;
    }
  }

  waterMap_.init(MAP_COLS, MAP_ROWS, std::move(tiles));
}

// ===========================================================================
// buildPropsMap
//   Props tileset: 2048x2048 = 32 cols x 32 rows of 64px tiles.
//   -1 = no prop on that cell.
//
//   Approximate tile IDs (from visual inspection of Props.png):
//     Sheet row 0 cols 0-2 = houses (blue, red, purple)
//     Sheet row 3 col 0    = bush
//     Sheet row 3 col 1    = tree (round)
//     Sheet row 3 col 2    = pine tree (small)
//     Sheet row 3 col 4    = large pine
//     Sheet row 3 col 5    = large oak
//     Sheet row 4 col 0    = rocks
//     Sheet row 4 col 1    = stump
//     Sheet row 4 col 2    = mushroom cluster
// ===========================================================================
void World::buildPropsMap() {
  std::vector<int> tiles(static_cast<size_t>(MAP_COLS * MAP_ROWS), -1);

  const int SHEET_COLS = 32;  // props sheet is 2048/64 = 32 cols wide
  auto id = [&](int sheetRow, int sheetCol) { return sheetRow * SHEET_COLS + sheetCol; };

  // Houses (upper-right quadrant of map)
  tiles[static_cast<size_t>(1 * MAP_COLS + 13)] = id(0, 0);  // blue house
  tiles[static_cast<size_t>(1 * MAP_COLS + 16)] = id(0, 1);  // red house
  tiles[static_cast<size_t>(2 * MAP_COLS + 19)] = id(0, 3);  // cave entrance

  // Shorthand tile IDs
  const int pine = id(3, 2);
  const int oak = id(3, 5);
  const int bush = id(3, 0);
  const int rocks = id(4, 0);
  const int stump = id(4, 1);
  const int mushroom = id(4, 2);

  // Top border trees
  for (int col : {0, 4, 8, 12, 15, 19}) {
    tiles[static_cast<size_t>(0 * MAP_COLS + col)] = pine;
  }
  // Bottom border trees
  for (int col : {1, 5, 9, 14, 18}) {
    tiles[static_cast<size_t>(14 * MAP_COLS + col)] = oak;
  }
  // Left column
  tiles[static_cast<size_t>(3 * MAP_COLS + 0)] = bush;
  tiles[static_cast<size_t>(5 * MAP_COLS + 0)] = oak;
  tiles[static_cast<size_t>(10 * MAP_COLS + 0)] = pine;
  // Right column
  tiles[static_cast<size_t>(4 * MAP_COLS + 19)] = bush;
  tiles[static_cast<size_t>(9 * MAP_COLS + 19)] = pine;
  tiles[static_cast<size_t>(12 * MAP_COLS + 19)] = oak;

  // Interior decorations
  tiles[static_cast<size_t>(3 * MAP_COLS + 15)] = rocks;
  tiles[static_cast<size_t>(5 * MAP_COLS + 5)] = stump;
  tiles[static_cast<size_t>(10 * MAP_COLS + 15)] = mushroom;
  tiles[static_cast<size_t>(11 * MAP_COLS + 3)] = bush;
  tiles[static_cast<size_t>(12 * MAP_COLS + 7)] = rocks;

  propsMap_.init(MAP_COLS, MAP_ROWS, std::move(tiles));
}

// ===========================================================================
// buildCollisionLayerFromColorMap
//   ColorImage mode: water and forest cells are solid, sand/grass passable.
// ===========================================================================
void World::buildCollisionLayerFromColorMap() {
  const int cols = colorMap_.cols();
  const int rows = colorMap_.rows();

  std::vector<bool> solid(static_cast<size_t>(cols * rows), false);
  for (int row = 0; row < rows; ++row) {
    for (int col = 0; col < cols; ++col) {
      solid[static_cast<size_t>(row * cols + col)] = colorMap_.isSolid(col, row);
    }
  }

  collision_.init(cols, rows, std::move(solid));
}

// ===========================================================================
// buildCollisionLayerFromTileset
//   Solid cells:
//     - Water pond (cols 2..6, rows 2..5)
//     - House tiles
//     - Border tree tiles
// ===========================================================================
void World::buildCollisionLayerFromTileset() {
  std::vector<bool> solid(static_cast<size_t>(MAP_COLS * MAP_ROWS), false);

  auto setSolid = [&](int col, int row) {
    if (col >= 0 && col < MAP_COLS && row >= 0 && row < MAP_ROWS)
      solid[static_cast<size_t>(row * MAP_COLS + col)] = true;
  };

  // Water pond
  for (int row = 2; row <= 5; ++row)
    for (int col = 2; col <= 6; ++col) setSolid(col, row);

  // Houses
  setSolid(13, 1);
  setSolid(16, 1);
  setSolid(19, 2);

  // Top border trees
  for (int col : {0, 4, 8, 12, 15, 19}) setSolid(col, 0);
  // Bottom border trees
  for (int col : {1, 5, 9, 14, 18}) setSolid(col, 14);
  // Left/right column props
  for (int row : {3, 5, 10}) setSolid(0, row);
  for (int row : {4, 9, 12}) setSolid(19, row);

  collision_.init(MAP_COLS, MAP_ROWS, std::move(solid));
}

// ===========================================================================
// buildObjectLayer
//   ColorImage mode: spawn the player on the first passable cell found while
//   spiralling out from the map centre (works for whatever shape the
//   painted island happens to be).
//   Tileset mode: hard-coded spawn points and trigger zones for the demo map.
// ===========================================================================
void World::buildObjectLayer() {
  if (source_ == MapSource::ColorImage) {
    const float DST = tileDstSize();
    const int cols = colorMap_.cols();
    const int rows = colorMap_.rows();

    int spawnCol = cols / 2;
    int spawnRow = rows / 2;

    const int maxRadius = std::max(cols, rows);
    bool found = false;
    for (int radius = 0; radius <= maxRadius && !found; ++radius) {
      for (int dy = -radius; dy <= radius && !found; ++dy) {
        for (int dx = -radius; dx <= radius && !found; ++dx) {
          const int col = cols / 2 + dx;
          const int row = rows / 2 + dy;
          if (col < 0 || col >= cols || row < 0 || row >= rows) continue;
          if (!colorMap_.isSolid(col, row)) {
            spawnCol = col;
            spawnRow = row;
            found = true;
          }
        }
      }
    }

    objectLayer_.addObject({"player_spawn", "spawn", spawnCol * DST, spawnRow * DST, DST, DST});
    return;
  }

  const float DST = static_cast<float>(TILE_SRC) * TILE_SCALE;

  // Player spawn at road intersection (col 10, row 7)
  objectLayer_.addObject({"player_spawn", "spawn", 10.0f * DST, 7.0f * DST, DST, DST});

  // Cave entrance trigger (right edge)
  objectLayer_.addObject({"cave_entrance", "trigger", 19.0f * DST, 2.0f * DST, DST, DST});

  // Town sign near the houses
  objectLayer_.addObject({"town_sign", "trigger", 13.0f * DST, 2.0f * DST, DST, DST});
}

bool World::canOccupy(float x, float y, float w, float h) const {
  const float tileW = tileDstSize();
  const float tileH = tileDstSize();
  return !collision_.overlaps(SDL_FRect{x, y, w, h}, tileW, tileH);
}

float World::mapWidth() const {
  return source_ == MapSource::ColorImage ? COLOR_GRID_COLS * COLOR_TILE_DST
                                          : MAP_COLS * TILE_SRC * TILE_SCALE;
}

float World::mapHeight() const {
  return source_ == MapSource::ColorImage ? COLOR_GRID_ROWS * COLOR_TILE_DST
                                          : MAP_ROWS * TILE_SRC * TILE_SCALE;
}

}  // namespace game::world
