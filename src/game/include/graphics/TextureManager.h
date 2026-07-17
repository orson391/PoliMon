#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "IRenderer.h"

namespace engine::graphics {

class TextureManager {
 public:
  static TextureManager& instance();

  bool initialize(::graphics::IRenderer& renderer);

  std::shared_ptr<::graphics::Texture> loadTexture(const std::string& name, const std::string& filename);

  std::shared_ptr<::graphics::Texture> getTexture(const std::string& name);

  void unloadTexture(const std::string& name);

  void clear();

 private:
  TextureManager() = default;
  ~TextureManager();

  TextureManager(const TextureManager&) = delete;
  TextureManager& operator=(const TextureManager&) = delete;

  ::graphics::IRenderer* renderer_ = nullptr;
  std::unordered_map<std::string, std::shared_ptr<::graphics::Texture>> textures_;
};

}  // namespace engine::graphics
