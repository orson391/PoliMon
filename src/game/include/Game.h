#pragma once

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <algorithm>
#include <cmath>
#include <memory>
#include <unordered_map>

#include "Application.h"

struct SDL_Renderer;

namespace game {
enum class Direction { Up, Down, Left, Right };
struct RGB {
  int r;
  int g;
  int b;
};

// RAII deleter so SDL_Texture is always released exactly once, regardless of
// which code path (normal shutdown, early return, exception) destroys the
// owning GameApp.
struct SDLTextureDeleter {
  void operator()(SDL_Texture* texture) const noexcept {
    if (texture) {
      SDL_DestroyTexture(texture);
    }
  }
};
using TexturePtr = std::unique_ptr<SDL_Texture, SDLTextureDeleter>;

class GameApp : public engine::Application {
 public:
  GameApp();
  ~GameApp() override = default;

  // Application already owns raw SDL_Window*/SDL_Renderer* handles, so
  // copying or moving a GameApp would double-free those resources. Disable
  // both explicitly (Rule of 5) instead of relying on the implicitly
  // generated (and unsafe) versions.
  GameApp(const GameApp&) = delete;
  GameApp& operator=(const GameApp&) = delete;
  GameApp(GameApp&&) = delete;
  GameApp& operator=(GameApp&&) = delete;

 protected:
  void onStart() override;
  void onUpdate(float dtSeconds) override;
  void onRender(SDL_Renderer* renderer) override;
  void onClose() override;
  void onConfigReload(const core::Config& newConfig) override;

 private:
  float hueToRGB(float p, float q, float t);
  RGB getRGBFromRectPosition(float posX, float posY, float rectX, float rectY, float rectW,
                             float rectH);

 private:
  float playerX_{100.0f};
  float playerY_{100.0f};
  float playerSpeed_{100.0f};
  bool isMoving_{false};

  Direction playerDirection_{Direction::Down};
  TexturePtr playerTexture_;
};

}  // namespace game
