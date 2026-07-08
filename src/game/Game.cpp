#include "Game.h"

#include "Logger.h"

namespace game {

GameApp::GameApp() {
  core::Logger::log("Game initialized");
}

void GameApp::onStart() {
  core::Logger::log("Game started");
}

void GameApp::onUpdate(float dtSeconds) {
  Application::onUpdate(dtSeconds);

  const bool* keys = SDL_GetKeyboardState(nullptr);
  isMoving_ = keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_UP] ||
              keys[SDL_SCANCODE_DOWN];

  if (keys[SDL_SCANCODE_LEFT]) {
    playerX_ -= playerSpeed_ * dtSeconds;
    playerDirection_ = Direction::Left;
  }
  if (keys[SDL_SCANCODE_RIGHT]) {
    playerX_ += playerSpeed_ * dtSeconds;
    playerDirection_ = Direction::Right;
  }
  if (keys[SDL_SCANCODE_UP]) {
    playerY_ -= playerSpeed_ * dtSeconds;
    playerDirection_ = Direction::Up;
  }
  if (keys[SDL_SCANCODE_DOWN]) {
    playerY_ += playerSpeed_ * dtSeconds;
    playerDirection_ = Direction::Down;
  }
}

void GameApp::onRender(SDL_Renderer* renderer) {
  if (!renderer) {
    return;
  }

  int imageWidth = 67, imageHeight = 67;
  int frameWidth = imageWidth / 4;
  int frameHeight = imageHeight / 4;

  int layerOne = 0, layerTwo = 0;

  playerTexture_ = nullptr;
  if (!playerTexture_) {
    playerTexture_ =
        IMG_LoadTexture(renderer, "C:\\Projects\\VsCode\\PoliMon\\build\\asset\\Player.png");
    if (!playerTexture_) {
      core::Logger::log("Failed to load player sprite: " + std::string(SDL_GetError()));
      return;
    }
  }

  int frameIndex = 0;
  switch (playerDirection_) {
    case Direction::Up:
      layerOne = 0;
      layerTwo = 0;
      break;
    case Direction::Down:
      layerOne = 0;
      layerTwo = 1;
      break;
    case Direction::Left:
      layerOne = 0;
      layerTwo = 2;
      break;
    case Direction::Right:
      layerOne = 0;
      layerTwo = 3;
      break;
  }

  if (isMoving_) {
    frameIndex = static_cast<int>(SDL_GetTicks() / 100) % 4;  // Change frame every 100 ms
    layerOne = frameIndex;
  }

  SDL_FRect src{static_cast<float>(layerOne * frameWidth),
                static_cast<float>(layerTwo * frameHeight), static_cast<float>(frameWidth),
                static_cast<float>(frameHeight)};
  SDL_FRect dst{playerX_, playerY_, 32.0f, 32.0f};
  SDL_RenderTexture(renderer, playerTexture_, &src, &dst);
}

void GameApp::onClose() {
  core::Logger::log("Game closed");
}

void GameApp::onConfigReload(const core::Config& newConfig) {
  core::Logger::log("Config reloaded: title='" + newConfig.title + "' size=" +
                    std::to_string(newConfig.width) + "x" + std::to_string(newConfig.height));
}

}  // namespace game
