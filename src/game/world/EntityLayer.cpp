#include "world/EntityLayer.h"
#include "world/Camera.h"

namespace game::world {

void EntityLayer::addEntity(std::unique_ptr<entities::Entity> entity) {
  entities_.push_back(std::move(entity));
}

void EntityLayer::update(float dt) {
  for (auto& entity : entities_) {
    entity->update(dt);
  }
}

void EntityLayer::render(SDL_Renderer* renderer, const Camera& camera) {
  for (auto& entity : entities_) {
    entity->render(renderer, camera);
  }
}

}  // namespace game::world
