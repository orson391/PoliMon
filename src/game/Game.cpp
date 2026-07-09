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

  auto& elem = rectangles_.at(4);
  RGB color = getRGBFromRectPosition(playerX_, playerY_, elem.x, elem.y, elem.width, elem.height);
  elem.r = color.r;
  elem.g = color.g;
  elem.b = color.b;
  elem.a = 255;
}

void GameApp::onRender(SDL_Renderer* renderer) {
  if (!renderer) {
    return;
  }

  for (const auto& pair : rectangles_) {
    float scaleX = pair.second.normalized ? static_cast<float>(width_) : 1.0f;
    float scaleY = pair.second.normalized ? static_cast<float>(height_) : 1.0f;

    SDL_FRect rect{pair.second.x * scaleX, pair.second.y * scaleY, pair.second.width * scaleX,
                   pair.second.height * scaleY};

    SDL_SetRenderDrawColorFloat(renderer, pair.second.r / 255.0f, pair.second.g / 255.0f,
                                pair.second.b / 255.0f, pair.second.a / 255.0f);

    if (pair.second.filled) {
      SDL_RenderFillRect(renderer, &rect);
    } else {
      SDL_RenderRect(renderer, &rect);
    }
  }

  constexpr int frameWidth = 16;
  constexpr int frameHeight = 16;
  constexpr int spacing = 1;

  // Load sprite sheet once.
  if (!playerTexture_) {
    SDL_Texture* loadedTexture =
        IMG_LoadTexture(renderer, "C:\\Projects\\VsCode\\PoliMon\\build\\asset\\Player.png");

    if (!loadedTexture) {
      core::Logger::log("Failed to load player sprite: " + std::string(SDL_GetError()));
      return;
    }

    playerTexture_.reset(loadedTexture);
  }

  // Determine sprite sheet row based on direction.
  int row = 0;
  switch (playerDirection_) {
    case Direction::Down:
      row = 0;
      break;

    case Direction::Left:
      row = 1;
      break;

    case Direction::Right:
      row = 2;
      break;

    case Direction::Up:
      row = 3;
      break;
  }

  // Animation frame (column).
  int column = 0;

  if (isMoving_) {
    column = (SDL_GetTicks() / 100) % 4;  // Change frame every 100 ms.
  }

  // Skip the 1-pixel spacing between sprites.
  SDL_FRect src{static_cast<float>(column * (frameWidth + spacing)),
                static_cast<float>(row * (frameHeight + spacing)), static_cast<float>(frameWidth),
                static_cast<float>(frameHeight)};

  // Draw player scaled to 32x32.
  SDL_FRect dst{playerX_, playerY_, 32.0f, 32.0f};

  SDL_RenderTexture(renderer, playerTexture_.get(), &src, &dst);
}
void GameApp::onClose() {
  core::Logger::log("Game closed");
}

void GameApp::onConfigReload(const core::Config& newConfig) {
  core::Logger::log("Config reloaded: title='" + newConfig.title + "' size=" +
                    std::to_string(newConfig.width) + "x" + std::to_string(newConfig.height));
}

float GameApp::hueToRGB(float p, float q, float t) {
  if (t < 0.0f) t += 1.0f;
  if (t > 1.0f) t -= 1.0f;
  if (t < 1.0f / 6.0f) return p + (q - p) * 6.0f * t;
  if (t < 1.0f / 2.0f) return q;
  if (t < 2.0f / 3.0f) return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
  return p;
}

RGB GameApp::getRGBFromRectPosition(float posX, float posY, float rectX, float rectY, float rectW,
                                    float rectH) {
  float centerX = rectX + (rectW / 2.0f);
  float centerY = rectY + (rectH / 2.0f);

  float maxRadius = std::min(rectW, rectH) / 2.0f;

  float dx = posX - centerX;
  float dy = posY - centerY;

  float radius = std::sqrt(dx * dx + dy * dy);

  float hue = std::atan2(dy, dx) * (180.0f / 3.1415f);
  if (hue < 0.0f) {
    hue += 360.0f;
  }

  float h = hue / 360.0f;
  float s = std::min(radius / maxRadius, 1.0f);  // Cap saturation at 100% outer rim
  float l = 0.5f;                                // 50% lightness yields rich, true hues

  float r = 0.0f, g = 0.0f, b = 0.0f;

  if (s == 0.0f) {
    r = g = b = l;  // Absolute center outputs neutral white/gray
  } else {
    float q = (l < 0.5f) ? (l * (1.0f + s)) : (l + s - l * s);
    float p = 2.0f * l - q;

    r = hueToRGB(p, q, h + 1.0f / 3.0f);
    g = hueToRGB(p, q, h);
    b = hueToRGB(p, q, h - 1.0f / 3.0f);
  }

  return {static_cast<int>(std::round(r * 255.0f)), static_cast<int>(std::round(g * 255.0f)),
          static_cast<int>(std::round(b * 255.0f))};
}

}  // namespace game
