#include "world/world.h"

#include <SDL3_image/SDL_image.h>

#include "Logger.h"

namespace game::world {

bool World::load(SDL_Renderer* renderer) {
  if (texture_) return true;

  SDL_Texture* tex = IMG_LoadTexture(renderer, "C:\\Projects\\VsCode\\PoliMon\\asset\\test.png");

  if (!tex) {
    core::Logger::log("Failed to load world texture: " + std::string(SDL_GetError()));

    return false;
  }

  texture_.reset(tex);

  return true;
}

void World::update(float dt) {
  (void)dt;
}

void World::render(SDL_Renderer* renderer) {
  if (!texture_) return;

  SDL_FRect dst{0.0f, 0.0f, 500.0f, 500.0f};

  SDL_RenderTexture(renderer, texture_.get(), nullptr, &dst);
}

}  // namespace game::world