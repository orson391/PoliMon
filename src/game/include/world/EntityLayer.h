#pragma once

#include <SDL3/SDL.h>

#include <memory>
#include <vector>

#include "entities/Entity.h"

namespace game::world {

// ---------------------------------------------------------------------------
// EntityLayer
//   Owns runtime Entity instances via unique_ptr.  Entities are added once
//   during World::load() and updated / rendered every frame.
//
//   This layer sits on top of all tile layers so entities appear above the
//   ground, water, and prop layers.
// ---------------------------------------------------------------------------
class Camera;

class EntityLayer {
 public:
   EntityLayer() = default;

   /// Transfer ownership of an entity into this layer.
   void addEntity(std::unique_ptr<entities::Entity> entity);

   void update(float dt);
   void render(SDL_Renderer* renderer, const Camera& camera);

  /// Non-owning access to all entities (e.g. for collision queries).
  const std::vector<std::unique_ptr<entities::Entity>>& entities() const {
    return entities_;
  }

 private:
  std::vector<std::unique_ptr<entities::Entity>> entities_;
};

}  // namespace game::world
