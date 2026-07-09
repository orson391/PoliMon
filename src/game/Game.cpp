#include "Game.h"

#include "Logger.h"

namespace game {

GameApp::GameApp() {
  core::Logger::log("Game initialized");
}

void GameApp::onStart() {
  core::Logger::log("Game started");

  if (!player_.load(renderer())) {
    core::Logger::log("Failed to load player.");
  }
}

void GameApp::onUpdate(float dtSeconds) {
  Application::onUpdate(dtSeconds);

  player_.update(dtSeconds);
}

void GameApp::onRender(SDL_Renderer* renderer) {
  if (!renderer) {
    return;
  }

  player_.render(renderer);
}

void GameApp::onClose() {
  core::Logger::log("Game closed");
}

void GameApp::onConfigReload(const core::Config& newConfig) {
  core::Logger::log("Config reloaded: title='" + newConfig.title + "' size=" +
                    std::to_string(newConfig.width) + "x" + std::to_string(newConfig.height));
}

}  // namespace game