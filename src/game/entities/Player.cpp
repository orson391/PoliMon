#include "entities/Player.h"

#include <SDL3_image/SDL_image.h>

#include "Logger.h"
#include "world/Camera.h"

namespace game::entities {

bool Player::load(SDL_Renderer* renderer) {
  if (texture_) {
    return true;
  }

  SDL_Texture* tex = IMG_LoadTexture(renderer, "C:\\Projects\\VsCode\\PoliMon\\asset\\Player.png");

  if (!tex) {
    core::Logger::log("Failed to load player sprite: " + std::string(SDL_GetError()));
    return false;
  }

  texture_.reset(tex);
  return true;
}

void Player::update(float dt) {
  const bool* keys = SDL_GetKeyboardState(nullptr);

  moving_ = false;

  if (keys[SDL_SCANCODE_LEFT]) {
    x_ -= speed_ * dt;
    direction_ = Direction::Left;
    moving_ = true;
  }

  if (keys[SDL_SCANCODE_RIGHT]) {
    x_ += speed_ * dt;
    direction_ = Direction::Right;
    moving_ = true;
  }

  if (keys[SDL_SCANCODE_UP]) {
    y_ -= speed_ * dt;
    direction_ = Direction::Up;
    moving_ = true;
  }

  if (keys[SDL_SCANCODE_DOWN]) {
    y_ += speed_ * dt;
    direction_ = Direction::Down;
    moving_ = true;
  }
}

void Player::render(SDL_Renderer* renderer, const world::Camera& camera) {
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

  SDL_FRect src{static_cast<float>(column * (frameWidth + spacing)),
                static_cast<float>(row * (frameHeight + spacing)), static_cast<float>(frameWidth),
                static_cast<float>(frameHeight)};

  const float zoom = camera.zoom();
  SDL_FRect dst{(x_ - camera.x()) * zoom, (y_ - camera.y()) * zoom, 32.0f * zoom, 32.0f * zoom};

  SDL_RenderTexture(renderer, texture_.get(), &src, &dst);
}

}  // namespace game::entities