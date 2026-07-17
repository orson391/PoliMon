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

bool TextureManager::initialize(::graphics::IRenderer& renderer) {
  renderer_ = &renderer;
  return true;
}

std::shared_ptr<::graphics::Texture> TextureManager::loadTexture(const std::string& name,
                                                                  const std::string& filename) {
  auto it = textures_.find(name);

  if (it != textures_.end()) return it->second;

  if (!renderer_) return nullptr;
  auto texture = renderer_->loadTexture(filename);

  if (!texture) {
    core::Logger::log("Failed to load texture: " + filename);
    return nullptr;
  }

  textures_[name] = texture;

  return texture;
}

std::shared_ptr<::graphics::Texture> TextureManager::getTexture(const std::string& name) {
  auto it = textures_.find(name);

  if (it == textures_.end()) return nullptr;

  return it->second;
}

void TextureManager::unloadTexture(const std::string& name) {
  textures_.erase(name);
}

void TextureManager::clear() {
  textures_.clear();
}

}  // namespace engine::graphics
