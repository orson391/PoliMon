#pragma once

#include "IRenderer.h"

namespace game::world {
class Camera;
}

namespace game::entities {

class Entity {
 public:
  virtual ~Entity() = default;

  virtual void update(float dt) = 0;
  virtual void render(::graphics::IRenderer& renderer, const world::Camera& camera) = 0;

 protected:
  float x_{};
  float y_{};
};

}  // namespace game::entities
