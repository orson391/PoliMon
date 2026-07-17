#include "Game.h"

#include <memory>

#include "Logger.h"
#include "entities/Player.h"
#include "graphics/TextureManager.h"
#include "ResourcePaths.h"

namespace game {

GameApp::GameApp() {
  core::Logger::log("Game initialized");
}

void GameApp::onStart() {
  core::Logger::log("Game started");

  engine::graphics::TextureManager::instance().initialize(renderer());

  if (!world_.load(renderer(), game::world::MapSource::Tileset)) {
    core::Logger::log("Failed to load world.");
  }

  auto& textures = engine::graphics::TextureManager::instance();

  textures.loadTexture("player", core::ResourcePaths::texture("Player.png").string());
  textures.loadTexture("tileset", core::ResourcePaths::texture("tileset.png").string());
  textures.loadTexture("objects", core::ResourcePaths::texture("objects.png").string());

  // Create Player and load its sprite
  auto player = std::make_unique<entities::Player>();
  if (!player->load()) {
    core::Logger::log("Failed to load player.");
  }

  // Position player at the spawn point defined in the object layer
  const auto* spawn = world_.objectLayer().findByName("player_spawn");
  if (spawn) {
    player->setPosition(spawn->x, spawn->y);
  }

  // Keep a raw pointer before transferring ownership
  auto* playerPtr = player.get();

  // Transfer ownership of Player into the EntityLayer
  world_.entityLayer().addEntity(std::move(player));

  // Connect player pointer to the world for camera tracking
  world_.setPlayer(playerPtr);
}

void GameApp::onUpdate(float dtSeconds) {
  Application::onUpdate(dtSeconds);
  world_.update(dtSeconds, width_, height_);
}

void GameApp::onRender(graphics::IRenderer& renderer) {
  world_.render(renderer);
}

void GameApp::onClose() {
  engine::graphics::TextureManager::instance().clear();
  core::Logger::log("Game closed");
}

void GameApp::onConfigReload(const core::Config& newConfig) {
  core::Logger::log("Config reloaded: title='" + newConfig.title + "' size=" +
                    std::to_string(newConfig.width) + "x" + std::to_string(newConfig.height));
}

}  // namespace game
