#include "IClonable.h"

namespace graphics::models {
class Rect : IClonable {
 public:
  float getWidth() const { return width_; };
  float getHeight() const { return height_; };
  float getX() const { return x_; };
  float getY() const { return y_; };
  float getLeft() const { return x_; };
  float getRight() const { return x_ + width_; };
  float getTop() const { return y_; };
  float getBottom() const { return y_ + height_; };

  void set(float x, float y, float width, float height) {
    x_ = x;
    y_ = y;
    width_ = width;
    height_ = height;
  }

  IClonable* clone() const override {
    Rect* r = new Rect();
    r->width_ = width_;
    r->height_ = height_;
    r->x_ = x_;
    r->y_ = y_;
    return r;
  }

 private:
  float width_;
  float height_;
  float x_;
  float y_;
};
}  // namespace graphics::models