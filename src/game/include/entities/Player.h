#pragma once

#include <memory>

#include "Entity.h"
#include "graphics/TextureManager.h"

namespace game::entities {

enum class Direction { Up, Down, Left, Right };

class Player : public Entity {
 public:
  Player() = default;

  bool load();

  void update(float dt) override;
  void render(::graphics::IRenderer& renderer, const world::Camera& camera) override;

  float x() const { return x_; }
  float y() const { return y_; }
  float width() const { return width_; }
  float height() const { return height_; }
  void setPosition(float x, float y) {
    x_ = x;
    y_ = y;
  }

  float desiredMoveX(float dt) const;
  float desiredMoveY(float dt) const;
  void commitMove(float dx, float dy);

 private:
  float speed_ = 100.0f;
  float pendingMoveX_ = 0.0f;
  float pendingMoveY_ = 0.0f;
  bool moving_ = false;
  Direction direction_ = Direction::Down;
  float width_ = 32.0f;
  float height_ = 32.0f;

  std::shared_ptr<::graphics::Texture> texture_;
};

}  // namespace game::entities
