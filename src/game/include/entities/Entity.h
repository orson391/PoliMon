#pragma once

#include <SDL3/SDL_render.h>

namespace game::entities {

class Entity {
 public:
  virtual ~Entity() = default;

  virtual void update(float dt) = 0;
  virtual void render(SDL_Renderer* renderer) = 0;

 protected:
  float x_{};
  float y_{};
};

}  // namespace game::entities