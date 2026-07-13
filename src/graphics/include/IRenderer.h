#include "models/Color.h"
#include "models/Rect.h"

namespace graphics {
class IRenderer {
 public:
  virtual ~IRenderer() = default;
  virtual void beginFrame() = 0;
  virtual void drawRect(const models::Rect& rect, models::Color color) = 0;
  virtual void endFrame() = 0;
};

}  // namespace graphics