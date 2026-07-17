#pragma once

#include <string>
#include <vector>

namespace game::world {

// ---------------------------------------------------------------------------
// MapObject
//   A named, typed, positioned rectangle — used for spawn points, triggers,
//   item locations, etc.  Does NOT own any texture; purely data.
// ---------------------------------------------------------------------------
struct MapObject {
  std::string name;
  std::string type;   // e.g. "spawn", "item", "trigger", "npc_spawn"
  float x = 0.0f;
  float y = 0.0f;
  float w = 0.0f;
  float h = 0.0f;
};

// ---------------------------------------------------------------------------
// ObjectLayer
//   Flat list of MapObjects. Supports adding objects and querying by name
//   or type.  In a future version this would be loaded from a Tiled TMX
//   object layer.
// ---------------------------------------------------------------------------
class ObjectLayer {
 public:
  ObjectLayer() = default;
  ObjectLayer(const ObjectLayer&) = default;
  ObjectLayer& operator=(const ObjectLayer&) = default;

  void addObject(MapObject obj);

  const std::vector<MapObject>& objects() const { return objects_; }

  /// Returns the first object with the given name, or nullptr.
  const MapObject* findByName(const std::string& name) const;

  /// Returns all objects whose type matches.
  std::vector<const MapObject*> findByType(const std::string& type) const;

 private:
  std::vector<MapObject> objects_;
};

}  // namespace game::world
