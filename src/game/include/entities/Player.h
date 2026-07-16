#pragma once

#include <memory>

#include "Entity.h"
#include "graphics/TextureManager.h"

namespace game::entities {

enum class Direction { Up, Down, Left, Right };

class Player : public Entity {
 public:
  Player() = default;

  bool load(SDL_Renderer* renderer);

  void update(float dt) override;
  void render(SDL_Renderer* renderer, const world::Camera& camera) override;

  float x() const { return x_; }
  float y() const { return y_; }
  void setPosition(float x, float y) {
    x_ = x;
    y_ = y;
  }

 private:
  float speed_ = 100.0f;
  bool moving_ = false;
  Direction direction_ = Direction::Down;

  SDL_Texture* texture_ = nullptr;
};

}  // namespace game::entities