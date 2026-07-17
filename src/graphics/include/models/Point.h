#include "IClonable.h"

namespace graphics::models {
class Point : IClonable {
 public:
  float getX() const { return x_; };
  float getY() const { return y_; };
  float getZ() const { return z_; };

  void set(float x, float y, float z = 0) {
    x_ = x;
    y_ = y;
    z_ = z;
  }

  IClonable* clone() const override {
    Point* p = new Point();
    p->x_ = x_;
    p->y_ = y_;
    p->z_ = z_;
    return p;
  }

 private:
  float z_;
  float x_;
  float y_;
};
}  // namespace graphics::models