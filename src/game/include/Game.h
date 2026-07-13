#pragma once

#include <SDL3/SDL.h>

#include "Application.h"
#include "entities/Player.h"
#include "world/world.h"

namespace game {

class GameApp : public engine::Application {
 public:
  GameApp();
  ~GameApp() override = default;

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
  entities::Player player_;
  world::World world_;
};

}  // namespace game