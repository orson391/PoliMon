#include "entities/Player.h"

#include <SDL3_image/SDL_image.h>

#include "Logger.h"
#include "world/Camera.h"

namespace game::entities {

bool Player::load() {
  texture_ = engine::graphics::TextureManager::instance().getTexture("player");

  if (!texture_) {
    core::Logger::log("Player texture not found.");
    return false;
  }

  return true;
}

void Player::update(float dt) {
  const bool* keys = SDL_GetKeyboardState(nullptr);

  moving_ = false;
  pendingMoveX_ = 0.0f;
  pendingMoveY_ = 0.0f;

  if (keys[SDL_SCANCODE_LEFT]) {
    pendingMoveX_ -= speed_ * dt;
    direction_ = Direction::Left;
    moving_ = true;
  }

  if (keys[SDL_SCANCODE_RIGHT]) {
    pendingMoveX_ += speed_ * dt;
    direction_ = Direction::Right;
    moving_ = true;
  }

  if (keys[SDL_SCANCODE_UP]) {
    pendingMoveY_ -= speed_ * dt;
    direction_ = Direction::Up;
    moving_ = true;
  }

  if (keys[SDL_SCANCODE_DOWN]) {
    pendingMoveY_ += speed_ * dt;
    direction_ = Direction::Down;
    moving_ = true;
  }
}

float Player::desiredMoveX(float /*dt*/) const {
  return pendingMoveX_;
}

float Player::desiredMoveY(float /*dt*/) const {
  return pendingMoveY_;
}

void Player::commitMove(float dx, float dy) {
  x_ += dx;
  y_ += dy;
}

void Player::render(::graphics::IRenderer& renderer, const world::Camera& camera) {
  if (!texture_) {
    return;
  }

  // Frustum culling: limit rendering distance for performance
  if (!camera.isVisible(x_, y_, 32.0f, 32.0f)) {
    return;
  }

  constexpr int frameWidth = 16;
  constexpr int frameHeight = 16;
  constexpr int spacing = 1;

  int row = 0;

  switch (direction_) {
    case Direction::Down:
      row = 0;
      break;
    case Direction::Up:
      row = 1;
      break;
    case Direction::Right:
      row = 2;
      break;
    case Direction::Left:
      row = 3;
      break;
  }

  int column = 0;

  if (moving_) {
    column = (SDL_GetTicks() / 100) % 4;
  }

  const ::graphics::Rect src{static_cast<float>(column * (frameWidth + spacing)),
                             static_cast<float>(row * (frameHeight + spacing)),
                             static_cast<float>(frameWidth), static_cast<float>(frameHeight)};

  const float zoom = camera.zoom();
  const ::graphics::Rect dst{(x_ - camera.x()) * zoom, (y_ - camera.y()) * zoom, 32.0f * zoom,
                             32.0f * zoom};

  renderer.drawTexture(*texture_, src, dst);
}

}  // namespace game::entities
