#pragma once

#include <SDL3/SDL.h>

#include <memory>

namespace game::world {

struct SDLTextureDeleter {
  void operator()(SDL_Texture* texture) const {
    if (texture) {
      SDL_DestroyTexture(texture);
    }
  }
};

class World {
 public:
  World() = default;

  bool load(SDL_Renderer* renderer);

  void update(float dt);
  void render(SDL_Renderer* renderer);

 private:
  std::unique_ptr<SDL_Texture, SDLTextureDeleter> texture_;
};

}  // namespace game::world