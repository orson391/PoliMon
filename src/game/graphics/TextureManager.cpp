#include "graphics/TextureManager.h"

#include "Logger.h"

namespace engine::graphics {

TextureManager& TextureManager::instance() {
  static TextureManager manager;
  return manager;
}

TextureManager::~TextureManager() {
  clear();
}

bool TextureManager::initialize(SDL_Renderer* renderer) {
  renderer_ = renderer;
  return renderer_ != nullptr;
}

SDL_Texture* TextureManager::loadTexture(const std::string& name, const std::string& filename) {
  auto it = textures_.find(name);

  if (it != textures_.end()) return it->second.get();

  SDL_Texture* texture = IMG_LoadTexture(renderer_, filename.c_str());

  if (!texture) {
    core::Logger::log(std::string("Failed to load texture: ") + filename + " : " + SDL_GetError());

    return nullptr;
  }

  textures_[name] = std::unique_ptr<SDL_Texture, TextureDeleter>(texture);

  return texture;
}

SDL_Texture* TextureManager::getTexture(const std::string& name) {
  auto it = textures_.find(name);

  if (it == textures_.end()) return nullptr;

  return it->second.get();
}

void TextureManager::unloadTexture(const std::string& name) {
  textures_.erase(name);
}

void TextureManager::clear() {
  textures_.clear();
}

}  // namespace engine::graphics