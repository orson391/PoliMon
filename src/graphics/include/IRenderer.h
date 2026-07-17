#pragma once

#include <memory>
#include <string>

namespace graphics {

namespace gl_context {
class Renderer;
}

struct Color {
  float r = 1.0f;
  float g = 1.0f;
  float b = 1.0f;
  float a = 1.0f;
};

struct Rect {
  float x = 0.0f;
  float y = 0.0f;
  float width = 0.0f;
  float height = 0.0f;
};

// GPU resource owned by the graphics layer. Game code never needs an OpenGL
// name; it only passes this object back to IRenderer when it wants to draw.
class Texture {
 public:
  ~Texture();

  int width() const { return width_; }
  int height() const { return height_; }

 private:
  friend class gl_context::Renderer;
  unsigned int id_ = 0;
  int width_ = 0;
  int height_ = 0;
};

class IRenderer {
 public:
  virtual ~IRenderer() = default;

  virtual void setViewport(int width, int height) = 0;
  virtual void beginFrame(Color clearColor) = 0;
  virtual void drawRect(const Rect& rect, Color color) = 0;
  virtual void drawTexture(const Texture& texture, const Rect& source, const Rect& destination) = 0;
  virtual void endFrame() = 0;

  virtual std::shared_ptr<Texture> loadTexture(const std::string& filename) = 0;
};

}  // namespace graphics
