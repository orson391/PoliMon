#pragma once

#include "IRenderer.h"

namespace graphics::gl_context {

class Renderer final : public IRenderer {
 public:
  Renderer() = default;
  ~Renderer() override;

  Renderer(const Renderer&) = delete;
  Renderer& operator=(const Renderer&) = delete;

  bool initialize(int width, int height);

  void setViewport(int width, int height) override;
  void beginFrame(Color clearColor) override;
  void drawRect(const Rect& rect, Color color) override;
  void drawTexture(const Texture& texture, const Rect& source, const Rect& destination) override;
  void endFrame() override;
  std::shared_ptr<Texture> loadTexture(const std::string& filename) override;

 private:
  bool createPipeline();
  void drawQuad(const Rect& destination, const Rect* source, const Texture* texture, Color color);

  unsigned int program_ = 0;
  unsigned int vao_ = 0;
  unsigned int vbo_ = 0;
  int viewportWidth_ = 1;
  int viewportHeight_ = 1;
  int viewportUniform_ = -1;
  int colorUniform_ = -1;
  int texturedUniform_ = -1;
};

}  // namespace graphics::gl_context
