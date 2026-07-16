#include "world/ObjectLayer.h"

#include <algorithm>

namespace game::world {

void ObjectLayer::addObject(MapObject obj) {
  objects_.push_back(std::move(obj));
}

const MapObject* ObjectLayer::findByName(const std::string& name) const {
  for (const auto& obj : objects_) {
    if (obj.name == name) return &obj;
  }
  return nullptr;
}

std::vector<const MapObject*> ObjectLayer::findByType(
    const std::string& type) const {
  std::vector<const MapObject*> result;
  for (const auto& obj : objects_) {
    if (obj.type == type) result.push_back(&obj);
  }
  return result;
}

}  // namespace game::world
