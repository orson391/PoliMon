#pragma once

#include <SDL3/SDL.h>

#include <memory>

#include "Entity.h"

namespace game::entities {

enum class Direction { Up, Down, Left, Right };

struct SDLTextureDeleter {
  void operator()(SDL_Texture* texture) const {
    if (texture) {
      SDL_DestroyTexture(texture);
    }
  }
};

class Player : public Entity {
 public:
  Player() = default;

  bool load(SDL_Renderer* renderer);

  void update(float dt) override;
  void render(SDL_Renderer* renderer) override;

  float x() const { return x_; }
  float y() const { return y_; }
  void setPosition(float x, float y) {
    x_ = x;
    y_ = y;
  }

 private:
  float speed_ = 50.0f;
  bool moving_ = false;
  Direction direction_ = Direction::Down;

  std::unique_ptr<SDL_Texture, SDLTextureDeleter> texture_;
};

}  // namespace game::entities