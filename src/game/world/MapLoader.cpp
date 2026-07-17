#include "world/MapLoader.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <sstream>

#include "Logger.h"

namespace game::world {

namespace {

std::string readFile(const std::string& path) {
  std::ifstream file(path);
  if (!file) return {};
  std::ostringstream ss;
  ss << file.rdbuf();
  return ss.str();
}

size_t findKey(const std::string& text, const std::string& key, size_t start = 0) {
  return text.find('"' + key + '"', start);
}

int readInt(const std::string& text, const std::string& key, int fallback) {
  const size_t pos = findKey(text, key);
  if (pos == std::string::npos) return fallback;
  const size_t colon = text.find(':', pos);
  if (colon == std::string::npos) return fallback;
  size_t end = colon + 1;
  while (end < text.size() && (std::isspace(static_cast<unsigned char>(text[end])) || text[end] == '+'))
    ++end;
  size_t parsedEnd = end;
  while (parsedEnd < text.size() && (std::isdigit(static_cast<unsigned char>(text[parsedEnd])) || text[parsedEnd] == '-'))
    ++parsedEnd;
  return std::stoi(text.substr(end, parsedEnd - end));
}

std::string readString(const std::string& text, const std::string& key, const std::string& fallback) {
  const size_t pos = findKey(text, key);
  if (pos == std::string::npos) return fallback;
  const size_t colon = text.find(':', pos);
  const size_t quote1 = text.find('"', colon + 1);
  const size_t quote2 = text.find('"', quote1 + 1);
  if (colon == std::string::npos || quote1 == std::string::npos || quote2 == std::string::npos) return fallback;
  return text.substr(quote1 + 1, quote2 - quote1 - 1);
}

std::vector<int> parseIntArray(const std::string& text, const std::string& key) {
  std::vector<int> values;
  const size_t pos = findKey(text, key);
  if (pos == std::string::npos) return values;
  const size_t lb = text.find('[', pos);
  const size_t rb = text.find(']', lb);
  if (lb == std::string::npos || rb == std::string::npos) return values;
  std::string token;
  for (size_t i = lb + 1; i < rb; ++i) {
    const char c = text[i];
    if ((c == '-') || std::isdigit(static_cast<unsigned char>(c))) {
      token.push_back(c);
    } else {
      if (!token.empty()) {
        values.push_back(std::stoi(token));
        token.clear();
      }
    }
  }
  if (!token.empty()) values.push_back(std::stoi(token));
  return values;
}

std::vector<MapObject> parseObjects(const std::string& text) {
  std::vector<MapObject> objects;
  const size_t pos = findKey(text, "objects");
  if (pos == std::string::npos) return objects;
  const size_t lb = text.find('[', pos);
  const size_t rb = text.find(']', lb);
  if (lb == std::string::npos || rb == std::string::npos) return objects;

  size_t cur = lb + 1;
  while (cur < rb) {
    const size_t objStart = text.find('{', cur);
    if (objStart == std::string::npos || objStart > rb) break;
    int depth = 0;
    size_t objEnd = objStart;
    for (; objEnd < rb; ++objEnd) {
      if (text[objEnd] == '{') ++depth;
      if (text[objEnd] == '}' && --depth == 0) {
        break;
      }
    }
    if (objEnd >= rb) break;

    const std::string chunk = text.substr(objStart, objEnd - objStart + 1);
    MapObject obj;
    obj.name = readString(chunk, "name", "");
    obj.type = readString(chunk, "type", "");
    obj.x = static_cast<float>(readInt(chunk, "x", 0));
    obj.y = static_cast<float>(readInt(chunk, "y", 0));
    obj.w = static_cast<float>(readInt(chunk, "width", 0));
    obj.h = static_cast<float>(readInt(chunk, "height", 0));
    objects.push_back(std::move(obj));
    cur = objEnd + 1;
  }
  return objects;
}

}  // namespace

bool MapLoader::loadFromFile(const std::string& path, MapData& out) {
  const std::string text = readFile(path);
  if (text.empty()) {
    core::Logger::log("MapLoader: failed to read '" + path + "'");
    return false;
  }

  out = MapData{};
  out.width = readInt(text, "width", 0);
  out.height = readInt(text, "height", 0);
  out.tileWidth = readInt(text, "tilewidth", 0);
  out.tileHeight = readInt(text, "tileheight", 0);

  out.tilesets = {
      {readString(text, "name", "Ground Tiles"), readString(text, "image", "Ground Tiles.png"),
       readInt(text, "firstgid", 1), out.tileWidth, out.tileHeight, readInt(text, "columns", 0)},
  };

  const auto objects = parseObjects(text);
  for (const auto& obj : objects) {
    out.objects.addObject(obj);
  }

  for (const auto& layerName : {"ground", "water", "props"}) {
    MapLayerData layer;
    layer.name = layerName;
    layer.tileset = std::string(layerName == std::string("ground") ? "Ground Tiles"
                             : layerName == std::string("water")  ? "Water Tiles"
                                                                  : "Props");
    layer.width = out.width;
    layer.height = out.height;
    layer.tiles = parseIntArray(text, layerName);
    out.layers.push_back(std::move(layer));
  }

  const auto collisionTiles = parseIntArray(text, "collision");
  out.collision.width = out.width;
  out.collision.height = out.height;
  out.collision.solid.reserve(collisionTiles.size());
  for (int value : collisionTiles) {
    out.collision.solid.push_back(value != 0);
  }

  return true;
}

}  // namespace game::world
