#pragma once

#include <string>
#include <vector>

#include "world/ObjectLayer.h"

namespace game::world {

struct MapLayerData {
  std::string name;
  std::string tileset;
  int width{0};
  int height{0};
  std::vector<int> tiles;
};

struct MapCollisionData {
  int width{0};
  int height{0};
  std::vector<bool> solid;
};

struct MapTilesetData {
  std::string name;
  std::string image;
  int firstGid{0};
  int tileWidth{0};
  int tileHeight{0};
  int columns{0};
};

struct MapData {
  int width{0};
  int height{0};
  int tileWidth{0};
  int tileHeight{0};

  std::vector<MapTilesetData> tilesets;
  std::vector<MapLayerData> layers;
  MapCollisionData collision;
  ObjectLayer objects;
};

class MapLoader {
 public:
  static bool loadFromFile(const std::string& path, MapData& out);
};

}  // namespace game::world
