#pragma once

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <memory>
#include <string>
#include <unordered_map>

namespace engine::graphics {

class TextureManager {
 public:
  static TextureManager& instance();

  bool initialize(SDL_Renderer* renderer);

  SDL_Texture* loadTexture(const std::string& name, const std::string& filename);

  SDL_Texture* getTexture(const std::string& name);

  void unloadTexture(const std::string& name);

  void clear();

 private:
  TextureManager() = default;
  ~TextureManager();

  TextureManager(const TextureManager&) = delete;
  TextureManager& operator=(const TextureManager&) = delete;

  struct TextureDeleter {
    void operator()(SDL_Texture* texture) const {
      if (texture) SDL_DestroyTexture(texture);
    }
  };

  SDL_Renderer* renderer_ = nullptr;

  std::unordered_map<std::string, std::unique_ptr<SDL_Texture, TextureDeleter> > textures_;
};

}  // namespace engine::graphics