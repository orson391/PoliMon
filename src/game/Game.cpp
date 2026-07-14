#include "Game.h"

#include <memory>

#include "Logger.h"
#include "entities/Player.h"

namespace game {

GameApp::GameApp() {
  core::Logger::log("Game initialized");
}

void GameApp::onStart() {
  core::Logger::log("Game started");

  // Load world. Currently using the flat-colour tilemap.png placeholder;
  // once real tile art is ready, switch to game::world::MapSource::Tileset
  // — nothing else in this function needs to change.
  if (!world_.load(renderer(), game::world::MapSource::ColorImage)) {
    core::Logger::log("Failed to load world.");
  }

  // Create Player and load its sprite
  auto player = std::make_unique<entities::Player>();
  if (!player->load(renderer())) {
    core::Logger::log("Failed to load player.");
  }

  // Position player at the spawn point defined in the object layer
  const auto* spawn = world_.objectLayer().findByName("player_spawn");
  if (spawn) {
    player->setPosition(spawn->x, spawn->y);
  }

  // Transfer ownership of Player into the EntityLayer
  world_.entityLayer().addEntity(std::move(player));
}

void GameApp::onUpdate(float dtSeconds) {
  Application::onUpdate(dtSeconds);
  world_.update(dtSeconds);
}

void GameApp::onRender(SDL_Renderer* renderer) {
  if (!renderer) return;
  world_.render(renderer);
}

void GameApp::onClose() {
  core::Logger::log("Game closed");
}

void GameApp::onConfigReload(const core::Config& newConfig) {
  core::Logger::log("Config reloaded: title='" + newConfig.title + "' size=" +
                    std::to_string(newConfig.width) + "x" + std::to_string(newConfig.height));
}

}  // namespace game