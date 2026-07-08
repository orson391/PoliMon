#pragma once

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "Application.h"

struct SDL_Renderer;

namespace game {
enum class Direction { Up, Down, Left, Right };

class GameApp : public engine::Application {
 public:
  GameApp();
  ~GameApp() = default;

 protected:
  void onStart() override;
  void onUpdate(float dtSeconds) override;
  void onRender(SDL_Renderer* renderer) override;
  void onClose() override;
  void onConfigReload(const core::Config& newConfig) override;

 private:
  float playerX_{100.0f};
  float playerY_{100.0f};
  float playerSpeed_{100.0f};
  bool isMoving_{false};

  Direction playerDirection_{Direction::Down};
  SDL_Texture* playerTexture_{nullptr};
};

}  // namespace game
