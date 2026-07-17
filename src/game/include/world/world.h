#pragma once

#include <SDL3/SDL.h>

#include "world/CollisionLayer.h"
#include "world/ColorTileMap.h"
#include "world/EntityLayer.h"
#include "world/ObjectLayer.h"
#include "world/TileMap.h"
#include "world/Tileset.h"
#include "world/Camera.h"

namespace game::entities {
class Player;
}

namespace game::world {

// ---------------------------------------------------------------------------
// MapSource
//   Which representation the world map should be built/rendered from.
//
//   ColorImage — reads tilemap.png and treats each flat colour as a tile
//                kind (water / sand / grass / forest). No tileset art
//                required — this is the placeholder mode.
//   Tileset    — the "real" mode: hard-coded tile-id grids drawn from actual
//                spritesheets (Ground/Water/Props tiles.png), same as before.
//
//   Swap modes by passing a different value to World::load(); nothing else
//   in Game.cpp needs to change.
// ---------------------------------------------------------------------------
enum class MapSource {
  ColorImage,
  Tileset,
};

// ---------------------------------------------------------------------------
// World
//   Top-level orchestrator that owns and sequences all layers:
//
//     Render order (back to front), ColorImage mode:
//       1. colorMap_    — single flat-colour grid built from tilemap.png
//       2. entityLayer_ — Player, NPCs, etc.
//
//     Render order (back to front), Tileset mode:
//       1. groundMap_   — grass / dirt / stone  (Ground Tiles.png)
//       2. waterMap_    — water bodies           (Water Tiles.png)
//       3. propsMap_    — trees, houses, rocks   (Props.png)
//       4. entityLayer_ — Player, NPCs, etc.
//
//   collision_ is a parallel grid marking which cells are solid, built from
//   whichever source is active.
//
//   objectLayer_ holds named spawn points and triggers set up in load().
// ---------------------------------------------------------------------------
class World {
 public:
  World() = default;

  /// Load and build the map using `source` (defaults to the colour-image
  /// placeholder — pass MapSource::Tileset once real tile art is ready).
  /// Returns false on any asset load failure (game continues with partial assets).
  bool load(SDL_Renderer* renderer, MapSource source = MapSource::ColorImage);

  void update(float dt, float viewportWidth, float viewportHeight);
  void render(SDL_Renderer* renderer);
  bool tryMove(entities::Player& player, float dx, float dy);

  void setPlayer(entities::Player* player) { player_ = player; }
  const Camera& camera() const { return camera_; }

  MapSource source() const { return source_; }

  /// Pixel size (width == height) of one tile in the *currently active*
  /// mode. Use this instead of a hard-coded constant when doing collision /
  /// movement math so it keeps working after switching MapSource.
  float tileDstSize() const;

  // ---- Layer accessors (for Game / systems that query world state) --------
  CollisionLayer& collisionLayer() { return collision_; }
  ObjectLayer& objectLayer() { return objectLayer_; }
  EntityLayer& entityLayer() { return entityLayer_; }

  const CollisionLayer& collisionLayer() const { return collision_; }
  const ObjectLayer& objectLayer() const { return objectLayer_; }
  const EntityLayer& entityLayer() const { return entityLayer_; }

 private:
  MapSource source_ = MapSource::ColorImage;

  // ---- ColorImage mode -----------------------------------------------------
  ColorTileMap colorMap_;

  // ---- Tileset mode: tilesets (one per spritesheet) ------------------------
  Tileset groundTileset_;
  Tileset waterTileset_;
  Tileset propsTileset_;

  // ---- Tileset mode: tile-map layers ----------------------------------------
  TileMap groundMap_;  // layer 0 — ground (always drawn)
  TileMap waterMap_;   // layer 1 — water bodies (-1 = transparent)
  TileMap propsMap_;   // layer 2 — props / decorations (-1 = transparent)

  // ---- Logical layers (shared by both modes) -------------------------------
  CollisionLayer collision_;
  ObjectLayer objectLayer_;
  EntityLayer entityLayer_;
  Camera camera_;
  entities::Player* player_ = nullptr;

  // ---- Private build helpers: ColorImage mode ------------------------------
  bool loadColorMap(SDL_Renderer* renderer);
  void buildCollisionLayerFromColorMap();

  // ---- Private build helpers: Tileset mode ---------------------------------
  bool loadTilesetAssets(SDL_Renderer* renderer);
  void buildGroundMap();
  void buildWaterMap();
  void buildPropsMap();
  void buildCollisionLayerFromTileset();

  // ---- Shared -----------------------------------------------------------
  void buildObjectLayer();
  bool canOccupy(float x, float y, float w, float h) const;
};

}  // namespace game::world
