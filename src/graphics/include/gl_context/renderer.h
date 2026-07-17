#include "IRenderer.h"

namespace graphics::gl_context {
class Renderer {
 public:
  Renderer() = default;
  ~Renderer() = default;

  void beginFrame();
  void drawRect(const models::Rect& rect, models::Color color);
  void endFrame();
}
}  // namespace graphics::gl_context